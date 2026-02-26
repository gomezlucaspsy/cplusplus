#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <random>
#include <functional>

#pragma comment(lib, "winmm.lib")

// ============================================================
//  CONSTANTS
// ============================================================
namespace {

const int   kScreenWidth            = 960;
const int   kScreenHeight           = 540;
const float kFov                    = 1.02f;
const float kHalfFov                = kFov * 0.5f;
const float kCameraPlaneScale       = std::tan(kHalfFov);
const float kMaxRayDistance         = 28.0f;
const float kPlayerRadius           = 0.28f;   // slightly larger for better feel
const float kGravity                = 18.5f;
const float kGroundFriction         = 10.0f;
const float kAirDrag                = 1.8f;
const float kAccelGround            = 21.0f;
const float kAccelAir               = 8.0f;
const float kTopRunSpeed            = 11.8f;
const float kBoostSpeed             = 16.5f;
const float kJumpVelocity           = 6.7f;
const float kTurnRate               = 2.65f;
const float kRingPickupDistance     = 0.65f;
const float kPlayerHitCooldownTime  = 0.85f;

// --- Homing missile ---
const float kEnemyMissileSpeed          = 7.8f;
const float kEnemyMissileLifetime       = 5.2f;
const float kEnemyMissileHitDistance    = 0.36f;
const float kEnemyMissileDespawnDist    = 18.0f;
const float kEnemyMissileTurnRate       = 3.1f;
const float kEnemyMissileWarmupMin      = 0.28f;
const float kEnemyMissileWarmupMax      = 0.66f;

// --- Drone enemy (NEW) ---
const float kDroneSpeed             = 4.5f;
const float kDroneDetectRadius      = 9.0f;
const float kDroneHitDistance       = 0.4f;
const float kDronePatrolChange      = 2.2f;  // seconds between new patrol dir

// --- Bouncer enemy (NEW) ---
const float kBouncerSpeed           = 5.5f;
const float kBouncerHitDistance     = 0.4f;
const float kFloatingSquareBobAmp   = 16.0f;
const float kFloatingSquareBobSpeed = 2.1f;
const float kFloatingSquareSpinSpeed= 1.45f;
const float kFloatingDonutBobAmp    = 18.0f;
const float kFloatingDonutBobSpeed  = 1.7f;
const float kFloatingDonutSpinA     = 1.1f;
const float kFloatingDonutSpinB     = 0.7f;

const int   kEnemyMissileMaxActive  = 7;
const int   kSnowParticleCount      = 220;
const float kSnowSpawnRadius        = 13.0f;
const int   kTargetFPS              = 60;

// ============================================================
//  DATA TYPES
// ============================================================
struct Vec2 { float x, y; };

struct Ring  { Vec2 pos; bool collected; };

struct Player {
    Vec2  pos, vel;
    float angle, z, vz;
    bool  onGround;
};

struct EnemyMissile {
    Vec2  pos, vel;
    float life, warmup, closestDistSq;
    bool  active;
};

// Drone: wanders until it sees the player, then charges
struct Drone {
    Vec2  pos, vel;
    float patrolTimer;
    bool  active;
    bool  alerted;
};

// Bouncer: bounces off walls in straight lines
struct Bouncer {
    Vec2  pos, vel;
    bool  active;
};

struct FloatingSquare {
    Vec2  pos;
    float phase;
    bool  active;
};

struct FloatingDonut {
    Vec2  pos;
    float phase;
    bool  active;
};

struct SnowParticle {
    Vec2  pos;
    float z, fallSpeed;
};

// ============================================================
//  MAP  (# = wall, o = ring, G = goal, S = spring, B = boost,
//         D = Drone spawn, E = Bouncer spawn, F = Floating square,
//         O = Floating donut)
// ============================================================
const std::vector<std::string> kMap = {
    "#....#######################",
    "#.....o..#.........o..G....#",
    "#.###.##.#.#######.#####.###",
    "#.#.....#.#.....#.....#...##",
    "#.###.#.#.#.###.#.###.#.#.##",
    "#...#.#...#...#...#...#.#..#",
    "###.#.#######.#####.###.##.#",
    "#...#.....o.#...#.OF..#....#",
    "#.#####.###.###.#.###.####.#",
    "#.....#.#.....#.#...#....#.#",
    "#.###.#.#.###.#.###.####.#.#",
    "#...#...#.#S#.#.....#..#.#.#",
    "###.#####.#.#.#######.##.#.#",
    "#...#.....#.#....o....#..#.D",
    "#.###.#####.#########.#.##.#",
    "#...#.....#.....B.....#....#",
    "#######################.E..#"
};

// ============================================================
//  GLOBALS
// ============================================================
std::vector<unsigned int> gPixels(kScreenWidth * kScreenHeight, 0);
std::vector<float>        gCameraX(kScreenWidth, 0.0f);

// Back-buffer DC for flicker-free rendering
HDC     gBackDC   = NULL;
HBITMAP gBackBmp  = NULL;
void*   gBackBits = nullptr;

std::mt19937 gRng(42);

// ============================================================
//  HELPERS
// ============================================================
int   mapWidth()  { return static_cast<int>(kMap.front().size()); }
int   mapHeight() { return static_cast<int>(kMap.size()); }

float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
float length(const Vec2& v)               { return std::sqrt(v.x*v.x + v.y*v.y); }
float lengthSq(const Vec2& v)             { return v.x*v.x + v.y*v.y; }

Vec2 normalizeOrZero(const Vec2& v) {
    float len = length(v);
    if (len < 0.0001f) return {0,0};
    return {v.x/len, v.y/len};
}
Vec2 add(const Vec2& a, const Vec2& b) { return {a.x+b.x, a.y+b.y}; }
Vec2 sub(const Vec2& a, const Vec2& b) { return {a.x-b.x, a.y-b.y}; }
Vec2 mul(const Vec2& v, float s)       { return {v.x*s, v.y*s}; }

Vec2 moveTowards(const Vec2& cur, const Vec2& tgt, float maxDelta) {
    Vec2 d = sub(tgt, cur);
    float dist = length(d);
    if (dist <= maxDelta || dist < 0.0001f) return tgt;
    return add(cur, mul(d, maxDelta/dist));
}

float randRange(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(gRng);
}

// ============================================================
//  MAP QUERY
// ============================================================
char mapCell(int x, int y) {
    if (x < 0 || y < 0 || y >= mapHeight() || x >= mapWidth()) return '#';
    return kMap[y][x];
}

// BUG FIX: was always returning false in the original
bool isWall(int x, int y) {
    if (x < 0 || y < 0 || y >= mapHeight() || x >= mapWidth()) {
        return true;
    }
    return false;
}

bool isWallF(float x, float y) {
    return isWall(static_cast<int>(x), static_cast<int>(y));
}

// ============================================================
//  PIXEL OUTPUT  (writes to gPixels only; blitted once per frame)
// ============================================================
inline void putPixel(int x, int y, unsigned int c) {
    if ((unsigned)x < (unsigned)kScreenWidth && (unsigned)y < (unsigned)kScreenHeight)
        gPixels[y * kScreenWidth + x] = c;
}

void fillRect(int x, int y, int w, int h, unsigned int c) {
    int x0=std::max(0,x), y0=std::max(0,y);
    int x1=std::min(kScreenWidth,x+w), y1=std::min(kScreenHeight,y+h);
    for (int py=y0; py<y1; ++py) {
        unsigned int* row = &gPixels[py*kScreenWidth];
        for (int px=x0; px<x1; ++px) row[px]=c;
    }
}

void clearScreen(unsigned int c) { std::fill(gPixels.begin(), gPixels.end(), c); }

void drawVerticalLine(int x, int y0, int y1, unsigned int c) {
    if ((unsigned)x >= (unsigned)kScreenWidth) return;
    int s=std::max(0,y0), e=std::min(kScreenHeight,y1);
    int idx = s*kScreenWidth+x;
    for (int y=s; y<e; ++y, idx+=kScreenWidth) gPixels[idx]=c;
}

void drawCircleOutline(int cx, int cy, int r, int thick, unsigned int c) {
    int o2=(r)*(r), i2=std::max(0,r-thick); i2*=i2;
    for (int dy=-r; dy<=r; ++dy)
        for (int dx=-r; dx<=r; ++dx) {
            int d2=dx*dx+dy*dy;
            if (d2<=o2 && d2>=i2) putPixel(cx+dx,cy+dy,c);
        }
}

void drawCircleFilled(int cx, int cy, int r, unsigned int c) {
    int r2=r*r;
    for (int dy=-r; dy<=r; ++dy)
        for (int dx=-r; dx<=r; ++dx)
            if (dx*dx+dy*dy<=r2) putPixel(cx+dx,cy+dy,c);
}

void drawLine(int x0, int y0, int x1, int y1, unsigned int c) {
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        putPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void initRayLut() {
    for (int x=0; x<kScreenWidth; ++x)
        gCameraX[x] = 2.0f*x/(float)kScreenWidth - 1.0f;
}

// ============================================================
//  BACK-BUFFER SETUP  (no more GetDC/ReleaseDC per frame)
// ============================================================
void createBackBuffer(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    gBackDC  = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = kScreenWidth;
    bmi.bmiHeader.biHeight      = -kScreenHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    gBackBmp = CreateDIBSection(gBackDC, &bmi, DIB_RGB_COLORS, &gBackBits, NULL, 0);
    SelectObject(gBackDC, gBackBmp);
    ReleaseDC(hWnd, hdc);
}

void presentFrame(HWND hWnd) {
    // Copy gPixels → DIB bits then BitBlt (fast, no stretch needed)
    std::memcpy(gBackBits, gPixels.data(), kScreenWidth * kScreenHeight * 4);
    HDC hdc = GetDC(hWnd);
    BitBlt(hdc, 0, 0, kScreenWidth, kScreenHeight, gBackDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hdc);
}

// ============================================================
//  SOUND  (simple beep-based effects via PlaySound / Beep)
// ============================================================
enum SoundId { SOUND_RING, SOUND_HIT, SOUND_JUMP, SOUND_WIN };

void playSound(SoundId id) {
    // Use MessageBeep tones via Beep() — no extra assets needed
    // Run in the calling thread (fast); won't block on modern hardware
    switch (id) {
        case SOUND_RING:  Beep(1200, 40);  break;
        case SOUND_HIT:   Beep(220,  120); break;
        case SOUND_JUMP:  Beep(880,  35);  break;
        case SOUND_WIN:   Beep(1047, 80); Beep(1319, 80); Beep(1568, 160); break;
    }
}

// ============================================================
//  WALL COLLISION  (IMPROVED — axis-separated sweep)
// ============================================================
void resolveWallCollision(Player& player) {
    const float r = kPlayerRadius;

    // X axis
    if (isWallF(player.pos.x + r, player.pos.y) ||
        isWallF(player.pos.x - r, player.pos.y)) {
        // Push out along X
        int tx = static_cast<int>(player.pos.x + (player.vel.x > 0 ? r : -r));
        float cx = tx + (player.vel.x > 0 ? 0.0f : 1.0f);
        player.pos.x = cx + (player.vel.x > 0 ? -r - 0.001f : r + 0.001f);
        player.vel.x *= -0.3f;  // slight bounce
    }

    // Y axis
    if (isWallF(player.pos.x, player.pos.y + r) ||
        isWallF(player.pos.x, player.pos.y - r)) {
        int ty = static_cast<int>(player.pos.y + (player.vel.y > 0 ? r : -r));
        float cy = ty + (player.vel.y > 0 ? 0.0f : 1.0f);
        player.pos.y = cy + (player.vel.y > 0 ? -r - 0.001f : r + 0.001f);
        player.vel.y *= -0.3f;
    }

    // Corner probe (diagonal cells) — keep player out of corners
    static const std::array<Vec2,4> corners = {
        Vec2{r, r}, Vec2{-r, r}, Vec2{r, -r}, Vec2{-r, -r}
    };
    for (auto& c : corners) {
        int cx = static_cast<int>(player.pos.x + c.x);
        int cy = static_cast<int>(player.pos.y + c.y);
        if (isWall(cx, cy)) {
            Vec2 away = normalizeOrZero({player.pos.x - (cx+0.5f), player.pos.y - (cy+0.5f)});
            if (length(away) < 0.0001f) away = {1,0};
            player.pos = add(player.pos, mul(away, 0.04f));
            player.vel = mul(player.vel, 0.75f);
        }
    }
}

// ============================================================
//  RAYCASTING
// ============================================================
float raycastDistance(const Player& player, float rdx, float rdy) {
    int mx = static_cast<int>(player.pos.x);
    int my = static_cast<int>(player.pos.y);

    float ddx = (std::fabs(rdx) < 0.0001f) ? 1e6f : std::fabs(1.0f/rdx);
    float ddy = (std::fabs(rdy) < 0.0001f) ? 1e6f : std::fabs(1.0f/rdy);

    int sx = rdx < 0 ? -1 : 1;
    int sy = rdy < 0 ? -1 : 1;
    float sdx = rdx < 0 ? (player.pos.x - mx)*ddx : (mx+1.0f - player.pos.x)*ddx;
    float sdy = rdy < 0 ? (player.pos.y - my)*ddy : (my+1.0f - player.pos.y)*ddy;

    bool side = false;
    for (;;) {
        if (sdx < sdy) { sdx+=ddx; mx+=sx; side=false; }
        else           { sdy+=ddy; my+=sy; side=true;  }
        if (isWall(mx,my)) break;
        if (std::min(sdx,sdy) > kMaxRayDistance) return kMaxRayDistance;
    }

    float dist = side
        ? (my - player.pos.y + (1-sy)*0.5f) / (rdy == 0.0f ? 0.0001f : rdy)
        : (mx - player.pos.x + (1-sx)*0.5f) / (rdx == 0.0f ? 0.0001f : rdx);
    return clampf(std::fabs(dist), 0.001f, kMaxRayDistance);
}

// ============================================================
//  OBJECT GATHER
// ============================================================
std::vector<Ring> gatherRings() {
    std::vector<Ring> rings;
    for (int y=0; y<mapHeight(); ++y)
        for (int x=0; x<mapWidth(); ++x)
            if (kMap[y][x]=='o')
                rings.push_back({{x+0.5f, y+0.5f}, false});
    return rings;
}

std::vector<Drone> gatherDrones() {
    std::vector<Drone> drones;
    for (int y=0; y<mapHeight(); ++y)
        for (int x=0; x<mapWidth(); ++x)
            if (kMap[y][x]=='D')
                drones.push_back({{x+0.5f, y+0.5f}, {0,0}, 0.0f, true, false});
    return drones;
}

std::vector<Bouncer> gatherBouncers() {
    std::vector<Bouncer> bouncers;
    for (int y=0; y<mapHeight(); ++y)
        for (int x=0; x<mapWidth(); ++x)
            if (kMap[y][x]=='E') {
                float angle = randRange(0.0f, 6.2831852f);
                bouncers.push_back({{x+0.5f, y+0.5f}, {std::cos(angle)*kBouncerSpeed, std::sin(angle)*kBouncerSpeed}, true});
            }
    return bouncers;
}

std::vector<FloatingSquare> gatherFloatingSquares() {
    std::vector<FloatingSquare> squares;
    for (int y=0; y<mapHeight(); ++y)
        for (int x=0; x<mapWidth(); ++x)
            if (kMap[y][x]=='F')
                squares.push_back({{x+0.5f, y+0.5f}, randRange(0.0f, 6.2831852f), true});
    return squares;
}

std::vector<FloatingDonut> gatherFloatingDonuts() {
    std::vector<FloatingDonut> donuts;
    for (int y=0; y<mapHeight(); ++y)
        for (int x=0; x<mapWidth(); ++x)
            if (kMap[y][x]=='O')
                donuts.push_back({{x+0.5f, y+0.5f}, randRange(0.0f, 6.2831852f), true});
    return donuts;
}

// ============================================================
//  MISSILE SPAWN
// ============================================================
void spawnEnemyMissile(const Player& player, std::vector<EnemyMissile>& missiles) {
    if ((int)missiles.size() >= kEnemyMissileMaxActive) return;

    Vec2 sp = player.pos;
    for (int attempt=0; attempt<8; ++attempt) {
        float angle = randRange(0.0f, 6.2831852f);
        float dist  = 9.0f + randRange(0.0f, 6.0f);
        sp = add(player.pos, {std::cos(angle)*dist, std::sin(angle)*dist});
        if (!isWallF(sp.x, sp.y)) break;
    }

    Vec2 lead = add(player.pos, mul(player.vel, 0.22f));
    Vec2 dir  = normalizeOrZero(sub(lead, sp));
    if (lengthSq(dir) < 0.0001f) dir = {1,0};

    EnemyMissile m;
    m.pos = sp;
    m.vel = mul(dir, kEnemyMissileSpeed);
    m.life = kEnemyMissileLifetime;
    m.warmup = randRange(kEnemyMissileWarmupMin, kEnemyMissileWarmupMax);
    m.closestDistSq = 1e9f;
    m.active = true;
    missiles.push_back(m);
}

// ============================================================
//  SNOW
// ============================================================
struct SnowParticle;

void respawnSnow(const Player& player, SnowParticle& p) {
    float angle  = randRange(0, 6.2831852f);
    float radius = randRange(0.5f, kSnowSpawnRadius);
    p.pos = add(player.pos, {std::cos(angle)*radius, std::sin(angle)*radius});
    p.z         = randRange(0.35f, 3.8f);
    p.fallSpeed = randRange(0.75f, 2.1f);
}

std::vector<SnowParticle> createSnow(const Player& player) {
    std::vector<SnowParticle> snow(kSnowParticleCount);
    for (auto& p : snow) respawnSnow(player, p);
    return snow;
}

void updateSnow(const Player& player, std::vector<SnowParticle>& snow, float dt, float t) {
    Vec2 wind = {std::sin(t*0.35f)*0.35f, std::cos(t*0.28f)*0.35f};
    for (auto& p : snow) {
        p.z -= p.fallSpeed * dt;
        p.pos = add(p.pos, mul(wind, dt));
        Vec2 d = sub(p.pos, player.pos);
        if (p.z <= 0.03f || lengthSq(d) > kSnowSpawnRadius*kSnowSpawnRadius)
            respawnSnow(player, p);
    }
}

// ============================================================
//  BILLBOARDS
// ============================================================
void drawRingBillboard(const Player& player, const Ring& ring, float horizon) {
    if (ring.collected) return;
    Vec2 d = sub(ring.pos, player.pos);
    float dist = length(d);
    if (dist < 0.15f || dist > 16.0f) return;
    float a = std::atan2(d.y, d.x) - player.angle;
    while (a > 3.14159f) a -= 6.28318f;
    while (a < -3.14159f) a += 6.28318f;
    if (std::fabs(a) > kFov*0.55f) return;
    int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int sz = (int)clampf(680.0f/dist, 4.0f, 120.0f);
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f);
    drawCircleOutline(sx, cy, sz/2, std::max(2,sz/6), 0x0000D9FF);
}

void drawEnemyMissileBillboard(const Player& player, const EnemyMissile& m, float horizon) {
    if (!m.active) return;
    Vec2 d = sub(m.pos, player.pos);
    float dist = length(d);
    if (dist < 0.1f || dist > 22.0f) return;
    float a = std::atan2(d.y, d.x) - player.angle;
    while (a > 3.14159f) a -= 6.28318f;
    while (a < -3.14159f) a += 6.28318f;
    if (std::fabs(a) > kHalfFov) return;
    int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int sz = (int)clampf(420.0f/dist, 3.0f, 44.0f);
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f);
    bool warn = m.warmup > 0.0f;
    drawCircleFilled(sx, cy, std::max(2,sz/2), warn ? 0x000095FF : 0x000033FF);
    drawCircleOutline(sx, cy, std::max(2,sz/2), 2,   warn ? 0x0000D9FF : 0x0000A0FF);
}

void drawDroneBillboard(const Player& player, const Drone& drone, float horizon) {
    if (!drone.active) return;
    Vec2 d = sub(drone.pos, player.pos);
    float dist = length(d);
    if (dist < 0.1f || dist > 20.0f) return;
    float a = std::atan2(d.y, d.x) - player.angle;
    while (a > 3.14159f) a -= 6.28318f;
    while (a < -3.14159f) a += 6.28318f;
    if (std::fabs(a) > kHalfFov) return;
    int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int sz = (int)clampf(500.0f/dist, 4.0f, 60.0f);
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f);
    // Drone floats at mid height — draw as a hexagon approximated by two rects + circle
    unsigned int col = drone.alerted ? 0x00FF3300 : 0x00FF8800;
    drawCircleFilled(sx, cy, std::max(3,sz/2), col);
    drawCircleOutline(sx, cy, std::max(3,sz/2), 2, 0x00FFFF00);
    // Rotor arms (small horizontal bar)
    fillRect(sx - sz/2, cy - 1, sz, 2, 0x00FFFF00);
}

void drawBouncerBillboard(const Player& player, const Bouncer& b, float horizon) {
    if (!b.active) return;
    Vec2 d = sub(b.pos, player.pos);
    float dist = length(d);
    if (dist < 0.1f || dist > 20.0f) return;
    float a = std::atan2(d.y, d.x) - player.angle;
    while (a > 3.14159f) a -= 6.28318f;
    while (a < -3.14159f) a += 6.28318f;
    if (std::fabs(a) > kHalfFov) return;
    int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int sz = (int)clampf(460.0f/dist, 4.0f, 52.0f);
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f);
    // Diamond shape (two triangles) — approximated with a rotated circle + outline
    drawCircleFilled(sx, cy, std::max(3,sz/2), 0x0000FF88);
    drawCircleOutline(sx, cy, std::max(3,sz/2), 2, 0x0000FFCC);
    fillRect(sx-1, cy - sz/2, 2, sz, 0x0000FFCC);  // vertical line
    fillRect(sx - sz/2, cy-1, sz, 2, 0x0000FFCC);   // horizontal line
}

void drawFloatingSquareBillboard(const Player& player, const FloatingSquare& sq, float horizon, float levelTimer) {
    if (!sq.active) return;
    Vec2 d = sub(sq.pos, player.pos);
    float dist = length(d);
    if (dist < 0.12f || dist > 20.0f) return;

    float a = std::atan2(d.y, d.x) - player.angle;
    while (a > 3.14159f) a -= 6.28318f;
    while (a < -3.14159f) a += 6.28318f;
    if (std::fabs(a) > kHalfFov) return;

    int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int sz = (int)clampf(540.0f/dist, 7.0f, 84.0f);
    float bob = std::sin(levelTimer * kFloatingSquareBobSpeed + sq.phase) * kFloatingSquareBobAmp;
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f - 42.0f + bob);

    float angle = levelTimer * kFloatingSquareSpinSpeed + sq.phase;
    float ca = std::cos(angle);
    float sa = std::sin(angle);
    float half = sz * 0.5f;

    std::array<Vec2,4> corners = {
        Vec2{-half, -half}, Vec2{half, -half}, Vec2{half, half}, Vec2{-half, half}
    };
    std::array<POINT,4> pts = {};
    for (int i=0; i<4; ++i) {
        float rx = corners[i].x * ca - corners[i].y * sa;
        float ry = corners[i].x * sa + corners[i].y * ca;
        pts[i] = {(LONG)(sx + rx), (LONG)(cy + ry)};
    }

    unsigned int edgeColor = 0x00FFFFFF;
    unsigned int innerColor = 0x009AF9FF;
    for (int i=0; i<4; ++i) {
        int n = (i + 1) & 3;
        drawLine(pts[i].x, pts[i].y, pts[n].x, pts[n].y, edgeColor);
    }

    int innerHalf = std::max(2, sz / 5);
    fillRect(sx - innerHalf, cy - innerHalf, innerHalf * 2, innerHalf * 2, innerColor);
}

void drawFloatingDonutBillboard(const Player& player, const FloatingDonut& donut, float horizon, float levelTimer) {
    if (!donut.active) return;
    Vec2 d = sub(donut.pos, player.pos);
    float dist = length(d);
    if (dist < 0.12f || dist > 22.0f) return;

    float viewAngle = std::atan2(d.y, d.x) - player.angle;
    while (viewAngle > 3.14159f) viewAngle -= 6.28318f;
    while (viewAngle < -3.14159f) viewAngle += 6.28318f;
    if (std::fabs(viewAngle) > kHalfFov) return;

    int sx = (int)((viewAngle/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
    int scale = (int)clampf(680.0f/dist, 9.0f, 96.0f);
    float bob = std::sin(levelTimer * kFloatingDonutBobSpeed + donut.phase) * kFloatingDonutBobAmp;
    int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f - 38.0f + bob);

    float a = levelTimer * kFloatingDonutSpinA + donut.phase;
    float b = levelTimer * kFloatingDonutSpinB + donut.phase * 0.7f;
    float ca = std::cos(a), sa = std::sin(a);
    float cb = std::cos(b), sb = std::sin(b);

    const float majorR = 1.65f;
    const float minorR = 0.72f;
    const int stepsI = 54;
    const int stepsJ = 20;

    for (int jj = 0; jj < stepsJ; ++jj) {
        float j = 6.2831852f * (float)jj / (float)stepsJ;
        float cj = std::cos(j), sj = std::sin(j);

        for (int ii = 0; ii < stepsI; ++ii) {
            float i = 6.2831852f * (float)ii / (float)stepsI;
            float ci = std::cos(i), si = std::sin(i);

            float ring = majorR + minorR * cj;
            float x = ring * ci;
            float y = ring * si;
            float z = minorR * sj;

            // Rotation inspired by the classic donut algorithm
            float x1 = x * ca - y * sa;
            float y1 = x * sa + y * ca;
            float z1 = z;

            float y2 = y1 * cb - z1 * sb;
            float z2 = y1 * sb + z1 * cb;

            float inv = 1.0f / (4.2f + z2);
            int px = sx + (int)(x1 * inv * scale);
            int py = cy + (int)(y2 * inv * scale * 0.66f);

            float shade = clampf((z2 + 1.2f) / 2.4f, 0.0f, 1.0f);
            unsigned int blue  = (unsigned int)(170 + 70 * shade);
            unsigned int green = (unsigned int)(90 + 80 * shade);
            unsigned int red   = (unsigned int)(30 + 40 * shade);
            unsigned int col = (blue << 16) | (green << 8) | red;
            putPixel(px, py, col);
        }
    }
}

void drawSnowBillboards(const Player& player, const std::vector<SnowParticle>& snow, float horizon) {
    for (auto& p : snow) {
        Vec2 d = sub(p.pos, player.pos);
        float dist = length(d);
        if (dist < 0.1f || dist > 18.0f) continue;
        float a = std::atan2(d.y, d.x) - player.angle;
        while (a > 3.14159f) a -= 6.28318f;
        while (a < -3.14159f) a += 6.28318f;
        if (std::fabs(a) > kHalfFov) continue;
        int sx = (int)((a/kHalfFov*0.5f+0.5f)*(float)kScreenWidth);
        int cy = (int)(kScreenHeight*0.5f + horizon - player.z*130.0f - p.z*120.0f);
        int sz = (int)clampf(56.0f/dist, 1.0f, 3.0f);
        drawCircleFilled(sx, cy, sz, 0x00FFFFFF);
    }
}

// ============================================================
//  HUD
// ============================================================
void drawHud(HDC hdc, int rings, int total, int speed, float timer,
             bool boosting, bool won, int lives, int dodged, bool lost,
             int drones, int bouncers) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));

    char hud[320];
    std::snprintf(hud, sizeof(hud),
        "RINGS %d/%d   SPEED %d   LIVES %d   DODGED %d   TIME %.1f%s%s%s",
        rings, total, speed, lives, dodged, timer,
        boosting ? "   BOOST"   : "",
        drones   ? "   DRONE!"  : "",
        bouncers ? "   BOUNCER!": "");
    TextOutA(hdc, 18, 16, hud, (int)std::strlen(hud));

    const char* help = "WASD Move | Left/Right Turn | Space Jump | Shift Boost | R Restart";
    TextOutA(hdc, 18, kScreenHeight-28, help, (int)std::strlen(help));

    if (lost) {
        RECT panel = {kScreenWidth/2-250, kScreenHeight/2-58, kScreenWidth/2+250, kScreenHeight/2+58};
        HBRUSH br = CreateSolidBrush(RGB(0,0,0));
        FillRect(hdc, &panel, br); DeleteObject(br);
        SetTextColor(hdc, RGB(255,70,70));
        const char* t = "YOU GOT HIT"; TextOutA(hdc, kScreenWidth/2-58, kScreenHeight/2-30, t,(int)strlen(t));
        SetTextColor(hdc, RGB(255,255,255));
        const char* l = "No lives left. Press R to restart."; TextOutA(hdc, kScreenWidth/2-118, kScreenHeight/2+4, l,(int)strlen(l));
        return;
    }
    if (won) {
        RECT panel = {kScreenWidth/2-250, kScreenHeight/2-58, kScreenWidth/2+250, kScreenHeight/2+58};
        HBRUSH br = CreateSolidBrush(RGB(0,0,0));
        FillRect(hdc, &panel, br); DeleteObject(br);
        SetTextColor(hdc, RGB(255,228,74));
        const char* t = "STAGE CLEAR!"; TextOutA(hdc, kScreenWidth/2-76, kScreenHeight/2-30, t,(int)strlen(t));
        SetTextColor(hdc, RGB(255,255,255));
        const char* l = "All rings collected. Reached the goal."; TextOutA(hdc, kScreenWidth/2-142, kScreenHeight/2+4, l,(int)strlen(l));
    }
}

// ============================================================
//  WINDOW PROC
// ============================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

} // namespace

// ============================================================
//  WINMAIN
// ============================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSA wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "SonicDoom3DWindow";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA("SonicDoom3DWindow", "Sonic Doom 3D (Improved)",
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,   // fixed window size
        CW_USEDEFAULT, CW_USEDEFAULT,
        kScreenWidth+16, kScreenHeight+39,
        NULL, NULL, hInstance, NULL);
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    createBackBuffer(hWnd);
    initRayLut();

    // --------------------------------------------------------
    // Game state
    // --------------------------------------------------------
    Player player = {{2.5f, 2.5f}, {0,0}, 0, 0, 0, true};

    std::vector<Ring>          rings;
    std::vector<EnemyMissile>  missiles;
    std::vector<Drone>         drones;
    std::vector<Bouncer>       bouncers;
    std::vector<FloatingSquare> floatingSquares;
    std::vector<FloatingDonut>  floatingDonuts;
    std::vector<SnowParticle>  snow;

    int   collectedRings    = 0;
    bool  won               = false;
    bool  lost              = false;
    float levelTimer        = 0.0f;
    int   lives             = 3;
    int   dodged            = 0;
    float missileSpawnTimer = 0.0f;
    float playerHitCooldown = 0.0f;
    bool  prevReset         = false;
    bool  prevJump          = false;

    auto resetGame = [&]() {
        player = {{2.5f,2.5f},{0,0},0,0,0,true};
        rings           = gatherRings();
        drones          = gatherDrones();
        bouncers        = gatherBouncers();
        floatingSquares = gatherFloatingSquares();
        floatingDonuts  = gatherFloatingDonuts();
        snow            = createSnow(player);
        missiles.clear();
        collectedRings  = 0;
        won=lost        = false;
        levelTimer      = 0;
        lives           = 3;
        dodged          = 0;
        missileSpawnTimer = 0;
        playerHitCooldown = 0;
    };
    resetGame();

    // --------------------------------------------------------
    // Timer — precise frame pacing with sleep
    // --------------------------------------------------------
    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    const float kFrameTime = 1.0f / (float)kTargetFPS;

    MSG msg = {};
    bool running = true;

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg); DispatchMessage(&msg);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - last.QuadPart) / (float)freq.QuadPart;

        // Frame limiter — sleep if we're ahead
        if (dt < kFrameTime) {
            DWORD sleepMs = (DWORD)((kFrameTime - dt) * 1000.0f);
            if (sleepMs > 1) Sleep(sleepMs - 1);
            continue;
        }
        last = now;
        dt = clampf(dt, 0.0f, 0.033f);
        levelTimer += dt;
        playerHitCooldown = std::max(0.0f, playerHitCooldown - dt);

        // ---- Reset ----
        bool resetNow = (GetAsyncKeyState('R') & 0x8000) != 0;
        if (resetNow && !prevReset) resetGame();
        prevReset = resetNow;

        if (!won && !lost) {
            // ---- Turning ----
            float turn = 0;
            if (GetAsyncKeyState(VK_LEFT)  & 0x8000) turn -= 1;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) turn += 1;
            player.angle += turn * kTurnRate * dt;

            Vec2 fwd = {std::cos(player.angle), std::sin(player.angle)};
            Vec2 rgt = {-fwd.y, fwd.x};

            // ---- Movement ----
            Vec2 mi = {0,0};
            if (GetAsyncKeyState('W') & 0x8000) mi = add(mi, fwd);
            if (GetAsyncKeyState('S') & 0x8000) mi = sub(mi, fwd);
            if (GetAsyncKeyState('D') & 0x8000) mi = add(mi, rgt);
            if (GetAsyncKeyState('A') & 0x8000) mi = sub(mi, rgt);
            mi = normalizeOrZero(mi);

            bool boosting   = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            float speedLim  = boosting ? kBoostSpeed : kTopRunSpeed;
            float accel     = player.onGround ? kAccelGround : kAccelAir;
            player.vel = moveTowards(player.vel, mul(mi, speedLim), accel*dt);

            float drag = player.onGround
                ? (lengthSq(mi) < 0.01f ? std::max(0.0f,1.0f-kGroundFriction*dt) : std::max(0.0f,1.0f-1.6f*dt))
                : std::max(0.0f,1.0f-kAirDrag*dt);
            player.vel = mul(player.vel, drag);

            // ---- Jump (edge-triggered with sound) ----
            bool jumpNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
            if (jumpNow && !prevJump && player.onGround) {
                player.vz = kJumpVelocity;
                player.onGround = false;
                playSound(SOUND_JUMP);
            }
            prevJump = jumpNow;

            // ---- Gravity ----
            player.vz -= kGravity * dt;
            player.z  += player.vz * dt;
            if (player.z <= 0) { player.z=0; player.vz=0; player.onGround=true; }

            // ---- Move & collide ----
            player.pos = add(player.pos, mul(player.vel, dt));
            resolveWallCollision(player);

            // ---- Map triggers ----
            int cx = (int)player.pos.x, cy = (int)player.pos.y;
            char cell = mapCell(cx, cy);
            if (cell=='S' && player.onGround) { player.vz=9.5f; player.onGround=false; playSound(SOUND_JUMP); }
            if (cell=='B' && player.onGround) { player.vel=add(player.vel, mul(fwd,9.5f)); }

            // ---- Ring pickup ----
            for (auto& r : rings) {
                if (!r.collected && lengthSq(sub(r.pos,player.pos)) <= kRingPickupDistance*kRingPickupDistance) {
                    r.collected = true;
                    ++collectedRings;
                    playSound(SOUND_RING);
                }
            }

            // ---- Goal ----
            if (cell=='G' && collectedRings == (int)rings.size()) {
                won = true;
                playSound(SOUND_WIN);
            }

            // ---- Homing missiles ----
            float spawnEvery = clampf(1.6f - levelTimer*0.014f, 0.55f, 1.6f);
            missileSpawnTimer += dt;
            if (missileSpawnTimer >= spawnEvery) { missileSpawnTimer=0; spawnEnemyMissile(player,missiles); }

            for (auto& m : missiles) {
                if (!m.active) continue;
                m.life -= dt;
                if (m.warmup > 0) { m.warmup=std::max(0.0f,m.warmup-dt); continue; }

                Vec2 desired = normalizeOrZero(sub(add(player.pos,mul(player.vel,0.2f)), m.pos));
                Vec2 curDir  = normalizeOrZero(m.vel);
                if (lengthSq(curDir) < 0.0001f) curDir = desired;
                Vec2 steered = normalizeOrZero(moveTowards(curDir, desired, clampf(kEnemyMissileTurnRate*dt,0,1)));
                m.vel = mul(steered, kEnemyMissileSpeed);
                m.pos = add(m.pos, mul(m.vel, dt));

                if (isWallF(m.pos.x, m.pos.y)) { m.active=false; continue; }

                float dsq = lengthSq(sub(player.pos, m.pos));
                m.closestDistSq = std::min(m.closestDistSq, dsq);
                if (playerHitCooldown <= 0 && dsq <= kEnemyMissileHitDistance*kEnemyMissileHitDistance) {
                    m.active=false; --lives;
                    player.vel=mul(player.vel,0.5f);
                    playerHitCooldown=kPlayerHitCooldownTime;
                    playSound(SOUND_HIT);
                    if (lives<=0) { lost=true; lives=0; }
                    continue;
                }
                if (m.life<=0 || dsq > kEnemyMissileDespawnDist*kEnemyMissileDespawnDist) {
                    m.active=false;
                    if (m.closestDistSq < 1.8f*1.8f) ++dodged;
                }
            }
            missiles.erase(std::remove_if(missiles.begin(),missiles.end(),[](auto& m){return !m.active;}),missiles.end());

            // ---- Drone enemies (NEW) ----
            for (auto& drone : drones) {
                if (!drone.active) continue;
                drone.patrolTimer -= dt;

                float toPlayerSq = lengthSq(sub(player.pos, drone.pos));
                bool canSee = toPlayerSq < kDroneDetectRadius * kDroneDetectRadius;

                if (canSee) {
                    // Chase
                    drone.alerted = true;
                    Vec2 dir = normalizeOrZero(sub(player.pos, drone.pos));
                    drone.vel = mul(dir, kDroneSpeed);
                } else if (drone.patrolTimer <= 0) {
                    // New random patrol direction (only if not hitting a wall)
                    drone.alerted = false;
                    float angle = randRange(0.0f, 6.2831852f);
                    drone.vel = {std::cos(angle)*kDroneSpeed*0.5f, std::sin(angle)*kDroneSpeed*0.5f};
                    drone.patrolTimer = kDronePatrolChange;
                }

                Vec2 newPos = add(drone.pos, mul(drone.vel, dt));
                if (!isWallF(newPos.x, newPos.y)) {
                    drone.pos = newPos;
                } else {
                    drone.vel = mul(drone.vel, -1.0f);  // bounce off wall
                    drone.patrolTimer = 0;
                }

                if (playerHitCooldown <= 0 && toPlayerSq <= kDroneHitDistance*kDroneHitDistance) {
                    --lives; playerHitCooldown=kPlayerHitCooldownTime;
                    player.vel=mul(player.vel,0.5f);
                    playSound(SOUND_HIT);
                    if (lives<=0) { lost=true; lives=0; }
                }
            }

            // ---- Bouncer enemies (NEW) ----
            for (auto& b : bouncers) {
                if (!b.active) continue;
                Vec2 newPos = add(b.pos, mul(b.vel, dt));

                // Reflect off walls
                Vec2 tryX = {newPos.x, b.pos.y};
                Vec2 tryY = {b.pos.x,  newPos.y};
                if (isWallF(tryX.x, tryX.y)) { b.vel.x *= -1.0f; newPos.x = b.pos.x; }
                if (isWallF(tryY.x, tryY.y)) { b.vel.y *= -1.0f; newPos.y = b.pos.y; }
                b.pos = newPos;

                float dsq = lengthSq(sub(player.pos, b.pos));
                if (playerHitCooldown <= 0 && dsq <= kBouncerHitDistance*kBouncerHitDistance) {
                    --lives; playerHitCooldown=kPlayerHitCooldownTime;
                    player.vel=mul(player.vel,0.5f);
                    playSound(SOUND_HIT);
                    if (lives<=0) { lost=true; lives=0; }
                }
            }
        } // !won && !lost

        updateSnow(player, snow, dt, levelTimer);

        // ---- Render ----
        float speed = length(player.vel);
        float bob   = std::sin(levelTimer*(6.0f+speed*0.7f)) * clampf(speed/24.0f,0,1) * 16.0f;
        float horizon = bob - player.z*120.0f;

        clearScreen(0x00000000);
        fillRect(0, 0, kScreenWidth, kScreenHeight/2+70, 0x00FFBE50);
        fillRect(0, kScreenHeight/2+(int)horizon, kScreenWidth, kScreenHeight/2+120, 0x00489117);

        Vec2 fwd = {std::cos(player.angle), std::sin(player.angle)};
        Vec2 plane = {-fwd.y*kCameraPlaneScale, fwd.x*kCameraPlaneScale};

        for (int x=0; x<kScreenWidth; ++x) {
            float rdx = fwd.x + plane.x*gCameraX[x];
            float rdy = fwd.y + plane.y*gCameraX[x];
            float dist = raycastDistance(player, rdx, rdy);
            float lh   = (float)kScreenHeight / dist;
            int ds = std::max(0, (int)(-lh/2 + kScreenHeight/2 + horizon));
            int de = std::min(kScreenHeight, (int)(lh/2 + kScreenHeight/2 + horizon));
            unsigned int shade = (unsigned int)clampf(255.0f - dist*14.0f, 25.0f, 255.0f);
            unsigned int wc    = ((shade&0xFF)<<16) | (((shade/2)&0xFF)<<8) | 0x0A;
            drawVerticalLine(x, ds, de, wc);
        }

        for (auto& r : rings)    drawRingBillboard(player, r, horizon);
        for (auto& m : missiles) drawEnemyMissileBillboard(player, m, horizon);
        for (auto& d : drones)   drawDroneBillboard(player, d, horizon);
        for (auto& b : bouncers) drawBouncerBillboard(player, b, horizon);
        for (auto& sq : floatingSquares) drawFloatingSquareBillboard(player, sq, horizon, levelTimer);
        for (auto& donut : floatingDonuts) drawFloatingDonutBillboard(player, donut, horizon, levelTimer);
        drawSnowBillboards(player, snow, horizon);

        // Count active enemies for HUD indicator
        int activeDrones = 0, activeBouncers = 0;
        for (auto& d : drones)   if (d.active && d.alerted) ++activeDrones;
        for (auto& b : bouncers) if (b.active) ++activeBouncers;

        // Present 3D scene
        presentFrame(hWnd);

        // Overlay HUD via GDI on top
        HDC hdc = GetDC(hWnd);
        drawHud(hdc, collectedRings, (int)rings.size(), (int)(speed*10),
                levelTimer, (GetAsyncKeyState(VK_SHIFT)&0x8000)!=0,
                won, lives, dodged, lost, activeDrones, activeBouncers);
        ReleaseDC(hWnd, hdc);
    }

    // Cleanup
    if (gBackBmp) DeleteObject(gBackBmp);
    if (gBackDC)  DeleteDC(gBackDC);
    return 0;
}
