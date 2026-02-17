#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>
#include <algorithm>
#include <memory>

// --- Constants & Config ---
constexpr int WIN_W = 1024;
constexpr int WIN_H = 768;
constexpr float PLAYER_SPEED = 600.0f;
constexpr float EXPLOSION_RATE = 40.0f;
constexpr float MAX_EXPLOSION_RADIUS = 40.0f;

// --- Math Helpers ---
struct Vec2 {
    float x, y;
    float dist(const Vec2& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
};

enum class EntityType { Enemy, PlayerMissile };

struct Entity {
    Vec2 pos;
    Vec2 target; // For player missiles
    Vec2 vel;
    bool active;
    bool exploding;
    float radius;
    EntityType type;
    
    // For enemies
    Entity(Vec2 p, Vec2 v) : pos(p), vel(v), active(true), exploding(false), radius(2.0f), type(EntityType::Enemy) {}
    
    // For player missiles - use a helper constructor with explicit type
    Entity(Vec2 start, Vec2 dest, int) : pos(start), target(dest), active(true), exploding(false), radius(2.0f), type(EntityType::PlayerMissile) {
        float dx = dest.x - start.x;
        float dy = dest.y - start.y;
        float d = std::sqrt(dx*dx + dy*dy);
        vel = { (dx/d) * PLAYER_SPEED, (dy/d) * PLAYER_SPEED };
    }
};

// --- Game Engine ---
class MissileCommand {
public:
    MissileCommand() : m_score(0), m_wave(1), m_gameOver(false) {
        m_base = {WIN_W / 2.0f, WIN_H - 30.0f};
        startWave();
    }

    void update(float dt) {
        if (m_gameOver) return;

        // 1. Update Player Missiles
        for (auto& m : m_missiles) {
            if (!m.active) continue;

            if (m.exploding) {
                // Expand explosion
                m.radius += EXPLOSION_RATE * dt;
                if (m.radius >= MAX_EXPLOSION_RADIUS) m.active = false;
            } else {
                // Move missile
                m.pos = m.pos + (m.vel * dt);
                
                // Check if reached target (simple distance check or overshooting logic)
                // If moving up, target y is less than current y usually
                if (m.pos.y <= m.target.y) {
                    m.exploding = true;
                }
            }
        }

        // 2. Update Enemies
        bool enemiesAlive = false;
        for (auto& e : m_enemies) {
            if (!e.active) continue;
            enemiesAlive = true;

            e.pos = e.pos + (e.vel * dt);

            // Ground hit
            if (e.pos.y > WIN_H) {
                e.active = false; // Hit ground
                // Reduce score or lives here if desired
            }

            // Base hit
            if (e.pos.dist(m_base) < 40.0f) {
                m_gameOver = true;
            }

            // Collision with Player Explosions
            for (const auto& m : m_missiles) {
                if (m.active && m.exploding) {
                    if (e.pos.dist(m.pos) < m.radius) {
                        e.active = false;
                        m_score += 10 * m_wave;
                        break; 
                    }
                }
            }
        }

        // 3. Cleanup & Wave Logic
        m_missiles.erase(std::remove_if(m_missiles.begin(), m_missiles.end(), 
            [](const Entity& e){ return !e.active; }), m_missiles.end());
            
        m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(), 
            [](const Entity& e){ return !e.active; }), m_enemies.end());

        if (!enemiesAlive && m_enemies.empty()) {
            m_wave++;
            startWave();
        }
    }

    void draw(HDC hdc) {
        // --- Double Buffering Setup ---
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WIN_W, WIN_H);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        // 1. Clear Background
        RECT rc = {0, 0, WIN_W, WIN_H};
        HBRUSH bgBrush = CreateSolidBrush(RGB(10, 10, 20)); // Dark Navy
        FillRect(memDC, &rc, bgBrush);
        DeleteObject(bgBrush);

        // 2. Draw Base
        drawCircle(memDC, m_base, 30, RGB(0, 255, 0), true);

        // 3. Draw Player Missiles (Trails and Explosions)
        for (const auto& m : m_missiles) {
            if (m.exploding) {
                drawCircle(memDC, m.pos, m.radius, RGB(255, 100 + (int)m.radius * 2, 0), true);
            } else {
                drawLine(memDC, m_base, m.pos, RGB(0, 0, 255));
                SetPixel(memDC, (int)m.pos.x, (int)m.pos.y, RGB(255, 255, 255));
            }
        }

        // 4. Draw Enemies
        for (const auto& e : m_enemies) {
            drawCircle(memDC, e.pos, 4, RGB(255, 0, 0), true);
            // Draw faint trail
            SetPixel(memDC, (int)e.pos.x, (int)e.pos.y - 2, RGB(100, 0, 0));
        }

        // 5. HUD
        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(200, 200, 200));
        wchar_t hud[128];
        wsprintf(hud, L"WAVE: %d  SCORE: %d", m_wave, m_score);
        TextOut(memDC, 10, 10, hud, (int)wcslen(hud));

        if (m_gameOver) {
            SetTextColor(memDC, RGB(255, 50, 50));
            const wchar_t* msg = L"GAME OVER - PRESS R TO RESTART";
            TextOut(memDC, WIN_W/2 - 100, WIN_H/2, msg, (int)wcslen(msg));
        }

        // --- Flip Buffer to Screen ---
        BitBlt(hdc, 0, 0, WIN_W, WIN_H, memDC, 0, 0, SRCCOPY);

        // Cleanup GDI
        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
    }

    void shoot(int x, int y) {
        if (!m_gameOver)
            m_missiles.emplace_back(m_base, Vec2{(float)x, (float)y}, 0);
    }

    void reset() {
        m_score = 0;
        m_wave = 1;
        m_gameOver = false;
        m_missiles.clear();
        m_enemies.clear();
        startWave();
    }

private:
    void startWave() {
        int count = 5 + (m_wave * 2);
        for (int i = 0; i < count; i++) {
            float sx = (float)(rand() % WIN_W);
            float sy = -20.0f - (rand() % 200); // Stagger spawns
            float vx = (rand() % 20 - 10) * 2.0f;
            float vy = (20.0f + (m_wave * 5.0f)); 
            m_enemies.emplace_back(Vec2{sx, sy}, Vec2{vx, vy});
        }
    }

    void drawCircle(HDC hdc, Vec2 p, float r, COLORREF c, bool filled) {
        HBRUSH brush = filled ? CreateSolidBrush(c) : (HBRUSH)GetStockObject(NULL_BRUSH);
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HGDIOBJ oldBrush = SelectObject(hdc, brush);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        
        Ellipse(hdc, (int)(p.x - r), (int)(p.y - r), (int)(p.x + r), (int)(p.y + r));
        
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
    }

    void drawLine(HDC hdc, Vec2 p1, Vec2 p2, COLORREF c) {
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        MoveToEx(hdc, (int)p1.x, (int)p1.y, nullptr);
        LineTo(hdc, (int)p2.x, (int)p2.y);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }

    Vec2 m_base;
    std::vector<Entity> m_missiles;
    std::vector<Entity> m_enemies;
    int m_score;
    int m_wave;
    bool m_gameOver;
};

// --- Windows Boilerplate ---
MissileCommand* g_game = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_game) g_game->draw(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
        if (g_game) g_game->shoot(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        return 0;
    case WM_KEYDOWN:
        if (wp == 'R' && g_game) g_game->reset();
        return 0;
    case WM_ERASEBKGND:
        return 1; // Prevent auto-clearing to reduce flicker
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.lpszClassName = L"MissileCmdClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"MissileCmdClass", L"Native Missile Command", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_W, WIN_H, 
        nullptr, nullptr, hInstance, nullptr);

    g_game = new MissileCommand();
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // High Resolution Timing
            auto now = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float>(now - lastTime).count();
            lastTime = now;

            // Cap dt to prevent spiraling physics if window drags
            if (dt > 0.05f) dt = 0.05f;

            g_game->update(dt);
            
            // Force Redraw
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }

    delete g_game;
    return 0;
}
