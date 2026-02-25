# Sonic Doom 3D (Native C++)

A lightweight **Sonic-inspired** game using **Doom-style 3D raycasting** logic in C++.

- Native C++ movement/physics loop
- Software-rendered raycast wall renderer (old-school 3D style)
- Sonic-like mechanics: speed, boost, springs, ring collection, goal gate

## Controls

- `W/A/S/D`: move
- `Left / Right`: turn
- `Space`: jump
- `Left Shift`: boost

## Map Markers

- `#` wall
- `o` ring collectible
- `S` spring (vertical launch)
- `B` boost pad
- `G` goal tile

## Build (Windows, MinGW, no external libs)

From this folder:

```powershell
g++ -std=c++14 -O2 -mwindows main.cpp -lgdi32 -o sonic_doom3d.exe
```

Then run:

```powershell
./sonic_doom3d.exe
```
