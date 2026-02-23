# Encrypted Communications - Complete Project Structure

## Directory Overview

```
encrypted_comm/
│
├── 📄 CMakeLists.txt              # Main build configuration (supports all platforms)
├── 📄 Makefile                    # Alternative build system (Unix-like)
├── 📄 README.md                   # Main documentation
├── 📄 INSTALL.md                  # Installation guide for all platforms
├── 📄 FREEBSD_INSTALL.md          # Specific FreeBSD installation guide
├── 📄 BSD_GUIDE.md                # Comprehensive BSD support guide
├── 📄 HOLYC_TEMPLEOS_GUIDE.md     # TempleOS/HolyC development guide
├── 📄 BSD_SUPPORT_SUMMARY.md      # BSD implementation overview
├── 📄 HOLYC_SUPPORT_SUMMARY.md    # HolyC implementation overview
├── 📄 PROJECT_STRUCTURE.md        # This file
├── 📄 build.sh                    # Unix/Linux/macOS build script
├── 📄 build.bat                   # Windows build script
├── 📄 build_freebsd.sh            # FreeBSD-specific build script
├── 📄 installer.nsi               # Windows NSIS installer configuration
│
├── 📁 include/                    # C++ Header files
│   ├── config.h                   # Configuration and constants
│   ├── encryption.h               # Encryption module interface
│   ├── network.h                  # Networking module interface
│   ├── ui.h                       # User interface module interface
│   └── utils.h                    # Utility functions interface
│
├── 📁 src/                        # C++ Source files
│   ├── main.cpp                   # Main application entry point
│   ├── encryption.cpp             # Encryption implementation
│   ├── network.cpp                # Networking implementation
│   ├── ui.cpp                     # UI implementation with MS-DOS styling
│   └── utils.cpp                  # Utility functions implementation
│
├── 📁 holyc/                      # HolyC implementation for TempleOS
│   ├── main.HC                    # Main application for TempleOS
│   ├── encryption.HC              # Encryption module (HolyC)
│   ├── network.HC                 # Network module (HolyC)
│   ├── ui.HC                      # UI module (HolyC)
│   └── README.md                  # HolyC directory documentation
│
├── 📁 FreeBSD_PORT/               # FreeBSD port files
│   └── Makefile                   # FreeBSD ports-compatible Makefile
│
└── 📁 build/                      # Build output directory (created during build)
    ├── encrypted_comm             # Compiled binary (Unix-like)
    ├── encrypted_comm.exe         # Compiled binary (Windows)
    └── [object files and libs]
```

## File Descriptions

### Configuration & Build Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Main CMake build configuration with platform detection for Windows, macOS, Linux, and all BSD variants |
| `Makefile` | Traditional Make build system for Unix-like systems |
| `build.sh` | Automated build script for Linux/macOS |
| `build.bat` | Automated build script for Windows with colored output |
| `build_freebsd.sh` | Specialized build script for FreeBSD with pkg integration |
| `installer.nsi` | NSIS script for creating Windows installer executable |

### Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Main project documentation with overview and quick start |
| `INSTALL.md` | Comprehensive installation guide for all platforms |
| `FREEBSD_INSTALL.md` | Detailed FreeBSD-specific installation and configuration |
| `BSD_GUIDE.md` | Complete BSD users guide with services, jails, performance tuning |
| `HOLYC_TEMPLEOS_GUIDE.md` | Comprehensive guide for TempleOS/HolyC development and usage |
| `BSD_SUPPORT_SUMMARY.md` | Summary of BSD implementation and features |
| `HOLYC_SUPPORT_SUMMARY.md` | Summary of HolyC/TempleOS implementation |
| `PROJECT_STRUCTURE.md` | This file - complete project overview |

### Header Files (`include/`)

| File | Purpose |
|------|---------|
| `config.h` | Global constants, version info, and color codes for MS-DOS styling |
| `encryption.h` | Password-based encryption module with key derivation |
| `network.h` | Cross-platform socket networking (TCP connections) |
| `ui.h` | Retro MS-DOS terminal user interface |
| `utils.h` | String utilities, validation, timestamps, IP detection |

### Source Files (`src/`)

| File | Functionality |
|------|---------------|
| `main.cpp` | Application entry point, menu system, message loop, encryption integration |
| `encryption.cpp` | XOR-based encryption with rotation, key derivation from password, nonce generation |
| `network.cpp` | Socket implementation for both client and server, message sending/receiving |
| `ui.cpp` | Terminal UI with retro green/cyan MS-DOS aesthetics, message display, input handling |
| `utils.cpp` | Hex conversion, validation, IP detection, file I/O, timestamp generation |

### BSD Port Integration

| File | Purpose |
|------|---------|
| `FreeBSD_PORT/Makefile` | FreeBSD ports collection compatible Makefile for pkg_create |

### HolyC/TempleOS Implementation

| File | Functionality |
|------|---------------|
| `holyc/main.HC` | Main application for TempleOS with menu and message loop |
| `holyc/encryption.HC` | Password-based encryption module (HolyC) |
| `holyc/network.HC` | Network/socket module for TempleOS (HolyC) |
| `holyc/ui.HC` | User interface with retro styling (HolyC) |
| `holyc/README.md` | Quick start and development guide for HolyC version |

## Supported Platforms

### Tier 1 (Primary Support)
- ✅ Windows (XP SP3+)
- ✅ Linux (glibc 2.17+)
- ✅ macOS (10.9+)
- ✅ FreeBSD (10.0+)
- ✅ **TempleOS (5.03+)** - HolyC implementation

### Tier 2 (Compatible)
- ✅ OpenBSD (6.0+)
- ✅ NetBSD (7.0+)
- ✅ DragonFly BSD (5.0+)

## Build System Comparison

### CMake (Recommended)
- **Pros**: Cross-platform, modern, well-maintained
- **Use**: All platforms
- **Command**: `cmake . && make`

### Make/Makefile
- **Pros**: Traditional, simple
- **Use**: Unix-like systems
- **Command**: `make && sudo make install`

### Batch Scripts
- **Pros**: Easy for Windows users
- **Use**: Windows
- **Command**: `build.bat`

### Shell Scripts
- **Pros**: Easy for Unix users
- **Use**: Linux, macOS, BSD
- **Command**: `./build.sh` or `./build_freebsd.sh`

## Installation Methods

```
User Environment
        ↓
    ┌───────────────────────────┐
    │ Choose Installation Method │
    └───────────────────────────┘
         ↙        ↓         ↓         ↘
    ┌──────┐  ┌──────┐  ┌────────┐  ┌──────────┐
    │Windows│  │ Unix │  │FreeBSD │  │TempleOS │
    └──────┘  └──────┘  └────────┘  └──────────┘
      ↓          ↓          ↓            ↓
    build.bat   build.sh   build_freebsd.sh #include
      ↓          ↓          ↓            ↓
    CMake      CMake      CMake/pkg     HolyC
      ↓          ↓          ↓            ↓
   NSIS      Install    Ports/Binary   Direct Run
  Installer   System-wide  System-wide   
```

## Feature Highlights

### Encryption
- Password-based key derivation (SHA1-inspired)
- XOR encryption with byte rotation
- Random nonce generation
- Platform-independent format

### Networking
- Cross-platform socket implementation
- TCP client/server architecture
- Thread-safe message handling
- Automatic connection detection

### User Interface
- Authentic MS-DOS look and feel
- ANSI color codes for all terminals
- Message history display
- Status bar with connection info
- Interactive menu system

### Cross-Platform Support
- Conditional compilation for platform-specific code
- Socket API abstraction (WinSock vs POSIX)
- Console color handling (Windows vs Unix)
- File and IP utilities

## Build Requirements

### Minimum
- C++17 compiler
- CMake 3.12 or Make
- Standard C++ libraries

### Optional
- NSIS (for Windows installer creation)
- pkg (FreeBSD package manager)

## Installation Targets

### Windows
- Portable .exe
- NSIS Installer with Start Menu integration
- Optional system-wide installation

### Unix-like (Linux/macOS)
- `/usr/local/bin/encrypted_comm`
- Manual uninstall via `make uninstall`

### FreeBSD
- System-wide via pkg or ports
- Service integration with rc scripts
- Jail compatibility

## Version Information

**Current Version**: 1.0.0
**Release Date**: February 2026
**C++ Standard**: C++17
**CMake Version**: 3.12+

## Next Steps for Users

1. **Quick Start**: See [README.md](README.md)
2. **Installation**: Follow [INSTALL.md](INSTALL.md)
3. **BSD Users**: Check [BSD_GUIDE.md](BSD_GUIDE.md)
4. **Development**: Review source code in `src/` and `include/`

## Development Notes

### Code Organization
- **Modular Design**: Each feature in separate files
- **Clear Interfaces**: Headers define public API
- **Cross-Platform Abstractions**: Platform-specific code isolated
- **Standard Library Only**: No external dependencies

### Building for Development
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./encrypted_comm
```

### Contributing
- Follow existing code style
- Test on multiple platforms
- Document platform-specific changes
- Update relevant installation guides

---

**For detailed information about any component, refer to the relevant documentation file above.**
