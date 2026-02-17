# Missile Command - Native Windows Edition

A lightweight, fast Atari Missile Command clone written in pure C++ using Windows API (GDI). No external dependencies required!

## Features
- **Fully Native**: Uses Windows API only - no external libraries
- **Optimized**: Written with performance in mind, minimal code footprint
- **Progressive Difficulty**: Waves get harder with more missiles
- **Simple Controls**: Click to shoot missiles at incoming enemies

## Game Rules
- Defend your base (green circle at bottom)
- Click anywhere to shoot a missile at that position
- Destroy red enemy missiles before they reach you
- Each wave has more enemies
- Game over if enemy hits your base

## Building

### Prerequisites
- MinGW with g++ installed
- Windows OS (for Windows API)

### Compile
```bash
g++ main.cpp -o missile_cmd -lgdi32 -luser32 -static
```

Or use the VS Code build task (Ctrl+Shift+B).

## How to Play
- **Mouse Move**: Aim crosshair (blue circle)
- **Click**: Fire missile toward cursor
- **Objective**: Clear all enemy missiles before wave ends
- **Score**: +10 base points per kill, +5 bonus per wave level

## Technical Details
- ~200 lines of optimized C++
- Single-header compatible (all code in one file)
- 60+ FPS gameplay
- Vector collision detection
- No heap fragmentation (object pooling)

Enjoy!
