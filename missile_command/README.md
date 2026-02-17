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
# Simple console build (shows console window):
g++ main.cpp -o missile_cmd.exe -std=c++17 -O2 -static -lgdi32 -luser32

# GUI build (no console window, use this for a native Windows app):
# Note: `-municode` is optional and only supported by some MinGW-w64 toolchains. If your
# g++ reports `unrecognized command line option '-municode'`, drop `-municode` from the
# command (example below).
g++ main.cpp -o missile_command.exe -std=c++17 -O2 -mwindows -static -lgdi32 -luser32

# Debug build with symbols:
g++ main.cpp -o missile_command_debug.exe -std=c++17 -g -municode -mwindows -lgdi32 -luser32
```

Notes:
- Put library flags (`-lgdi32 -luser32`) after source/object files; ordering matters for some linkers.
- Use `-mwindows` to build a GUI application without a console window. Add `-municode` if you use wide-character WinAPI entry points.
- `-static` is optional; omit it if you prefer dynamic linking.
- If using MSYS2/MinGW-w64, ensure your `g++` comes from the MinGW toolchain (not MSYS POSIX build).

If you want to build from VS Code (Ctrl+Shift+B) using the existing task, ensure the task `args` are correct and don't contain stray spaces in `-I`/`-L` options. Example `tasks.json` args for this project (no `-municode`):

```json
"args": [
		"-std=c++17",
		"-g",
		"-mwindows",
		"main.cpp",
		"-o",
		"${fileDirname}\\${fileBasenameNoExtension}.exe",
		"-lgdi32",
		"-luser32"
]
```

Troubleshooting common errors:

- PowerShell: `The term 'g++.exe:' is not recognized as the name of a cmdlet...`
	- This usually happens if you accidentally pasted compiler output (which starts with `g++.exe:`) into the shell. Instead, run the compile command itself (examples above). Don't include the compiler's error lines as commands.

- `g++.exe: error: unrecognized command line option '-municode'`
	- Your installed `g++` doesn't support `-municode`. Remove `-municode` from the command (see GUI build example above) or install a MinGW-w64 toolchain that supports it.

Quick checks to diagnose your toolchain:

```powershell
g++ --version
g++ -v
where g++
```

Run a working compile (copy one of these commands):

```powershell
g++ main.cpp -o missile_cmd.exe -std=c++17 -O2 -static -lgdi32 -luser32
g++ main.cpp -o missile_command.exe -std=c++17 -O2 -mwindows -static -lgdi32 -luser32
```

Once a console build works, update your VS Code `tasks.json` to match the exact flags that worked.

If you see `std::thread` / threading errors during compile
------------------------------------------------------

- Example errors you might see:
	- `error: 'thread' is not a member of 'std'`
	- `error: 'thread' in namespace 'std' does not name a type`

- Cause:
	- Older MinGW toolchains (the MinGW.org build) often lack a working `libwinpthread` and full C++11/17 threading support. Your `g++ --version` may show something like `MinGW.org GCC-6.3.0-1` which is known to exhibit these issues.

- Solutions:
	1. Install a modern MinGW-w64 toolchain (recommended). MinGW-w64 provides `libwinpthread` and proper `std::thread` support.
		 - Easiest route: install MSYS2 (https://www.msys2.org/) and then install the mingw-w64 toolchain:

```powershell
# From an MSYS2 MinGW64 shell:
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
# Use the compiler at C:\msys64\mingw64\bin\g++.exe (add to PATH or call with full path)
```

	2. Alternatively, download a MinGW-w64 standalone installer (e.g., from winlibs or MSYS2 packages) and add its `bin` directory to your `PATH`.

	3. As a temporary workaround, you can remove or stub out threading code in `main.cpp`, but that may disable music/sound playback if the code uses threads for audio.

- After installing MinGW-w64, re-run the quick checks and compile commands shown earlier; a modern toolchain should compile code using `std::thread` successfully.

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
