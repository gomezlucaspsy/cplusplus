#include <windows.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

namespace {
const int kScreenWidth = 960;
const int kScreenHeight = 540;
const float kFov = 1.02f;
const float kMaxRayDistance = 28.0f;
const float kPlayerRadius = 0.22f;
const float kGravity = 18.5f;
const float kGroundFriction = 10.0f;
const float kAirDrag = 1.8f;
const float kAccelGround = 21.0f;
const float kAccelAir = 8.0f;
const float kTopRunSpeed = 11.8f;
const float kBoostSpeed = 16.5f;
const float kJumpVelocity = 6.7f;
const float kTurnRate = 2.65f;
const float kRingPickupDistance = 0.65f;

struct Vec2 {
    float x;
    float y;
};

struct Ring {
    Vec2 pos;
    bool collected;
};

struct Player {
    Vec2 pos;
    Vec2 vel;
    float angle;
    float z;
    float vz;
    bool onGround;
};

const std::vector<std::string> kMap = {
    "############################",
    "#.....o..#.........o..G....#",
    "#.###.##.#.#######.#####.###",
    "#.#.....#.#.....#.....#...##",
    "#.###.#.#.#.###.#.###.#.#.##",
    "#...#.#...#...#...#...#.#..#",
    "###.#.#######.#####.###.##.#",
    "#...#.....o.#...#.....#....#",
    "#.#####.###.###.#.###.####.#",
    "#.....#.#.....#.#...#....#.#",
    "#.###.#.#.###.#.###.####.#.#",
    "#...#...#.#S#.#.....#..#.#.#",
    "###.#####.#.#.#######.##.#.#",
    "#...#.....#.#....o....#..#.#",
    "#.###.#####.#########.#.##.#",
    "#...#.....#.....B.....#....#",
    "############################"
};

std::vector<unsigned int> gPixels(kScreenWidth * kScreenHeight, 0);

int mapWidth() { return static_cast<int>(kMap.front().size()); }
int mapHeight() { return static_cast<int>(kMap.size()); }

float clampf(float value, float minv, float maxv) {
    return std::max(minv, std::min(maxv, value));
}

float length(const Vec2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 normalizeOrZero(const Vec2& v) {
    const float len = length(v);
    if (len < 0.0001f) {
        Vec2 out = {0.0f, 0.0f};
        return out;
    }
    Vec2 out = {v.x / len, v.y / len};
    return out;
}

Vec2 add(const Vec2& a, const Vec2& b) {
    Vec2 out = {a.x + b.x, a.y + b.y};
    return out;
}

Vec2 sub(const Vec2& a, const Vec2& b) {
    Vec2 out = {a.x - b.x, a.y - b.y};
    return out;
}

Vec2 mul(const Vec2& v, float scalar) {
    Vec2 out = {v.x * scalar, v.y * scalar};
    return out;
}

bool isWall(int x, int y) {
    if (x < 0 || y < 0 || y >= mapHeight() || x >= mapWidth()) {
        return true;
    }
    return kMap[y][x] == '#';
}

char mapCell(int x, int y) {
    if (x < 0 || y < 0 || y >= mapHeight() || x >= mapWidth()) {
        return '#';
    }
    return kMap[y][x];
}

void putPixel(int x, int y, unsigned int color) {
    if (x < 0 || y < 0 || x >= kScreenWidth || y >= kScreenHeight) {
        return;
    }
    gPixels[y * kScreenWidth + x] = color;
}

void fillRect(int x, int y, int w, int h, unsigned int color) {
    const int x0 = std::max(0, x);
    const int y0 = std::max(0, y);
    const int x1 = std::min(kScreenWidth, x + w);
    const int y1 = std::min(kScreenHeight, y + h);

    for (int py = y0; py < y1; ++py) {
        unsigned int* row = &gPixels[py * kScreenWidth];
        for (int px = x0; px < x1; ++px) {
            row[px] = color;
        }
    }
}

void clearScreen(unsigned int color) {
    std::fill(gPixels.begin(), gPixels.end(), color);
}

void drawCircleOutline(int cx, int cy, int radius, int thickness, unsigned int color) {
    const int outer = radius;
    const int inner = std::max(0, radius - thickness);
    const int outer2 = outer * outer;
    const int inner2 = inner * inner;

    for (int y = -outer; y <= outer; ++y) {
        for (int x = -outer; x <= outer; ++x) {
            const int d2 = x * x + y * y;
            if (d2 <= outer2 && d2 >= inner2) {
                putPixel(cx + x, cy + y, color);
            }
        }
    }
}

void resolveWallCollision(Player& player) {
    std::array<Vec2, 4> probes = {
        Vec2{player.pos.x + kPlayerRadius, player.pos.y},
        Vec2{player.pos.x - kPlayerRadius, player.pos.y},
        Vec2{player.pos.x, player.pos.y + kPlayerRadius},
        Vec2{player.pos.x, player.pos.y - kPlayerRadius},
    };

    for (int pass = 0; pass < 2; ++pass) {
        for (size_t i = 0; i < probes.size(); ++i) {
            const int tx = static_cast<int>(probes[i].x);
            const int ty = static_cast<int>(probes[i].y);
            if (!isWall(tx, ty)) {
                continue;
            }

            const float cx = tx + 0.5f;
            const float cy = ty + 0.5f;
            Vec2 away = {player.pos.x - cx, player.pos.y - cy};
            away = normalizeOrZero(away);
            if (length(away) < 0.0001f) {
                away.x = 1.0f;
                away.y = 0.0f;
            }

            player.pos = add(player.pos, mul(away, 0.03f));
            player.vel = mul(player.vel, 0.82f);
        }

        probes[0] = Vec2{player.pos.x + kPlayerRadius, player.pos.y};
        probes[1] = Vec2{player.pos.x - kPlayerRadius, player.pos.y};
        probes[2] = Vec2{player.pos.x, player.pos.y + kPlayerRadius};
        probes[3] = Vec2{player.pos.x, player.pos.y - kPlayerRadius};
    }
}

float raycastDistance(const Player& player, float rayAngle) {
    const float rayDirX = std::cos(rayAngle);
    const float rayDirY = std::sin(rayAngle);

    int mapX = static_cast<int>(player.pos.x);
    int mapY = static_cast<int>(player.pos.y);

    const float deltaDistX = (std::fabs(rayDirX) < 0.0001f) ? 1e6f : std::fabs(1.0f / rayDirX);
    const float deltaDistY = (std::fabs(rayDirY) < 0.0001f) ? 1e6f : std::fabs(1.0f / rayDirY);

    int stepX = 0;
    int stepY = 0;
    float sideDistX = 0.0f;
    float sideDistY = 0.0f;

    if (rayDirX < 0.0f) {
        stepX = -1;
        sideDistX = (player.pos.x - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0f - player.pos.x) * deltaDistX;
    }

    if (rayDirY < 0.0f) {
        stepY = -1;
        sideDistY = (player.pos.y - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0f - player.pos.y) * deltaDistY;
    }

    bool hit = false;
    bool side = false;
    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = false;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = true;
        }

        if (isWall(mapX, mapY)) {
            hit = true;
        }

        const float traveledX = (std::fabs(rayDirX) < 0.0001f) ? 0.0f : std::fabs((mapX - player.pos.x) / rayDirX);
        const float traveledY = (std::fabs(rayDirY) < 0.0001f) ? 0.0f : std::fabs((mapY - player.pos.y) / rayDirY);
        if (traveledX > kMaxRayDistance && traveledY > kMaxRayDistance) {
            return kMaxRayDistance;
        }
    }

    float dist = 0.0f;
    if (!side) {
        dist = (mapX - player.pos.x + (1 - stepX) * 0.5f) / ((rayDirX == 0.0f) ? 0.0001f : rayDirX);
    } else {
        dist = (mapY - player.pos.y + (1 - stepY) * 0.5f) / ((rayDirY == 0.0f) ? 0.0001f : rayDirY);
    }
    return clampf(std::fabs(dist), 0.001f, kMaxRayDistance);
}

std::vector<Ring> gatherRings() {
    std::vector<Ring> rings;
    for (int y = 0; y < mapHeight(); ++y) {
        for (int x = 0; x < mapWidth(); ++x) {
            if (kMap[y][x] == 'o') {
                Ring ring;
                ring.pos.x = static_cast<float>(x) + 0.5f;
                ring.pos.y = static_cast<float>(y) + 0.5f;
                ring.collected = false;
                rings.push_back(ring);
            }
        }
    }
    return rings;
}

void drawRingBillboard(const Player& player, const Ring& ring, float horizonOffset) {
    if (ring.collected) {
        return;
    }

    const Vec2 toRing = sub(ring.pos, player.pos);
    const float dist = length(toRing);
    if (dist < 0.15f || dist > 16.0f) {
        return;
    }

    float ringAngle = std::atan2(toRing.y, toRing.x) - player.angle;
    while (ringAngle > 3.1415926f) ringAngle -= 6.2831852f;
    while (ringAngle < -3.1415926f) ringAngle += 6.2831852f;

    if (std::fabs(ringAngle) > kFov * 0.5f) {
        return;
    }

    const float nx = ringAngle / (kFov * 0.5f);
    const int screenX = static_cast<int>((nx * 0.5f + 0.5f) * static_cast<float>(kScreenWidth));
    const int projectedSize = static_cast<int>(clampf(680.0f / dist, 4.0f, 120.0f));
    const int centerY = static_cast<int>(kScreenHeight * 0.5f + horizonOffset - player.z * 130.0f);

    drawCircleOutline(screenX, centerY, projectedSize / 2, std::max(2, projectedSize / 6), 0x0000D9FF);
}

void drawHud(HDC hdc, int rings, int totalRings, int speed, float timer, bool boosting, bool won) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    char hud[256];
    std::snprintf(hud, sizeof(hud), "RINGS %d/%d   SPEED %d   TIME %.1f%s", rings, totalRings, speed, timer, boosting ? "   BOOST" : "");
    TextOutA(hdc, 18, 16, hud, static_cast<int>(std::strlen(hud)));

    const char* help = "WASD Move | Left/Right Turn | Space Jump | Shift Boost";
    TextOutA(hdc, 18, kScreenHeight - 28, help, static_cast<int>(std::strlen(help)));

    if (won) {
        RECT panel = {kScreenWidth / 2 - 250, kScreenHeight / 2 - 58, kScreenWidth / 2 + 250, kScreenHeight / 2 + 58};
        HBRUSH panelBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &panel, panelBrush);
        DeleteObject(panelBrush);

        SetTextColor(hdc, RGB(255, 228, 74));
        const char* clear = "STAGE CLEAR!";
        TextOutA(hdc, kScreenWidth / 2 - 76, kScreenHeight / 2 - 30, clear, static_cast<int>(std::strlen(clear)));

        SetTextColor(hdc, RGB(255, 255, 255));
        const char* line = "All rings collected. Reached the goal.";
        TextOutA(hdc, kScreenWidth / 2 - 142, kScreenHeight / 2 + 4, line, static_cast<int>(std::strlen(line)));
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

}  // namespace

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSA wc;
    std::memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SonicDoom3DWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA(
        "SonicDoom3DWindow",
        "Sonic Doom 3D (Native C++)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        kScreenWidth + 16,
        kScreenHeight + 39,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);

    BITMAPINFO bmi;
    std::memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = kScreenWidth;
    bmi.bmiHeader.biHeight = -kScreenHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    Player player;
    player.pos.x = 2.5f;
    player.pos.y = 2.5f;
    player.vel.x = 0.0f;
    player.vel.y = 0.0f;
    player.angle = 0.0f;
    player.z = 0.0f;
    player.vz = 0.0f;
    player.onGround = true;

    std::vector<Ring> rings = gatherRings();
    int collectedRings = 0;
    bool won = false;
    float levelTimer = 0.0f;

    LARGE_INTEGER perfFreq;
    LARGE_INTEGER lastCounter;
    QueryPerformanceFrequency(&perfFreq);
    QueryPerformanceCounter(&lastCounter);

    MSG msg;
    bool running = true;

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        float dt = static_cast<float>(currentCounter.QuadPart - lastCounter.QuadPart) / static_cast<float>(perfFreq.QuadPart);
        lastCounter = currentCounter;
        dt = clampf(dt, 0.0f, 0.033f);
        levelTimer += dt;

        Vec2 forward = {std::cos(player.angle), std::sin(player.angle)};
        Vec2 right = {-forward.y, forward.x};

        float turnInput = 0.0f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) turnInput -= 1.0f;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) turnInput += 1.0f;
        player.angle += turnInput * kTurnRate * dt;

        Vec2 moveInput = {0.0f, 0.0f};
        if (GetAsyncKeyState('W') & 0x8000) moveInput = add(moveInput, forward);
        if (GetAsyncKeyState('S') & 0x8000) moveInput = sub(moveInput, forward);
        if (GetAsyncKeyState('D') & 0x8000) moveInput = add(moveInput, right);
        if (GetAsyncKeyState('A') & 0x8000) moveInput = sub(moveInput, right);
        moveInput = normalizeOrZero(moveInput);

        const bool boosting = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const float accel = player.onGround ? kAccelGround : kAccelAir;
        const float speedLimit = boosting ? kBoostSpeed : kTopRunSpeed;

        player.vel = add(player.vel, mul(moveInput, accel * dt));
        const float speed = length(player.vel);
        if (speed > speedLimit) {
            player.vel = mul(player.vel, speedLimit / speed);
        }

        if (player.onGround) {
            const float friction = std::max(0.0f, 1.0f - kGroundFriction * dt);
            if (length(moveInput) < 0.1f) {
                player.vel = mul(player.vel, friction);
            } else {
                player.vel = mul(player.vel, std::max(0.0f, 1.0f - 3.2f * dt));
            }
        } else {
            player.vel = mul(player.vel, std::max(0.0f, 1.0f - kAirDrag * dt));
        }

        if (player.onGround && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            player.vz = kJumpVelocity;
            player.onGround = false;
        }

        player.vz -= kGravity * dt;
        player.z += player.vz * dt;

        if (player.z <= 0.0f) {
            player.z = 0.0f;
            player.vz = 0.0f;
            player.onGround = true;
        }

        player.pos = add(player.pos, mul(player.vel, dt));
        resolveWallCollision(player);

        const int cellX = static_cast<int>(player.pos.x);
        const int cellY = static_cast<int>(player.pos.y);
        const char cell = mapCell(cellX, cellY);

        if (cell == 'S' && player.onGround) {
            player.vz = 9.5f;
            player.onGround = false;
        }

        if (cell == 'B' && player.onGround) {
            player.vel = add(player.vel, mul(forward, 9.5f));
        }

        for (size_t i = 0; i < rings.size(); ++i) {
            if (rings[i].collected) {
                continue;
            }
            if (length(sub(rings[i].pos, player.pos)) <= kRingPickupDistance) {
                rings[i].collected = true;
                ++collectedRings;
            }
        }

        if (cell == 'G' && collectedRings == static_cast<int>(rings.size())) {
            won = true;
        }

        const float movementSpeed = length(player.vel);
        const float bob = std::sin(levelTimer * (6.0f + movementSpeed * 0.7f)) * clampf(movementSpeed / 24.0f, 0.0f, 1.0f) * 16.0f;
        const float horizonOffset = bob - player.z * 120.0f;

        clearScreen(0x00000000);
        fillRect(0, 0, kScreenWidth, kScreenHeight / 2 + 70, 0x00FFBE50);
        fillRect(0, kScreenHeight / 2 + static_cast<int>(horizonOffset), kScreenWidth, kScreenHeight / 2 + 120, 0x00489117);

        for (int x = 0; x < kScreenWidth; ++x) {
            const float cameraX = 2.0f * x / static_cast<float>(kScreenWidth) - 1.0f;
            const float rayAngle = player.angle + cameraX * (kFov * 0.5f);
            const float rawDistance = raycastDistance(player, rayAngle);
            const float correctedDistance = rawDistance * std::cos(cameraX * (kFov * 0.5f));
            const float lineHeight = static_cast<float>(kScreenHeight) / correctedDistance;

            int drawStart = static_cast<int>(-lineHeight / 2.0f + kScreenHeight / 2.0f + horizonOffset);
            drawStart = std::max(0, drawStart);
            int drawEnd = static_cast<int>(lineHeight / 2.0f + kScreenHeight / 2.0f + horizonOffset);
            drawEnd = std::min(kScreenHeight, drawEnd);

            const unsigned int shade = static_cast<unsigned int>(clampf(255.0f - correctedDistance * 14.0f, 25.0f, 255.0f));
            const unsigned int wallColor = ((shade & 0xFF) << 16) | (((shade / 2) & 0xFF) << 8) | 0x0A;
            fillRect(x, drawStart, 1, drawEnd - drawStart, wallColor);
        }

        for (size_t i = 0; i < rings.size(); ++i) {
            drawRingBillboard(player, rings[i], horizonOffset);
        }

        HDC hdc = GetDC(hWnd);
        StretchDIBits(
            hdc,
            0,
            0,
            kScreenWidth,
            kScreenHeight,
            0,
            0,
            kScreenWidth,
            kScreenHeight,
            &gPixels[0],
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY);

        drawHud(hdc, collectedRings, static_cast<int>(rings.size()), static_cast<int>(movementSpeed * 10.0f), levelTimer, boosting, won);

        ReleaseDC(hWnd, hdc);
    }

    return 0;
}
