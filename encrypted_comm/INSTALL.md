# Installation Guide

## Quick Installation

### Windows (Easiest Method)

#### Option 1: Using the Installer (Recommended)
1. Download `EncryptedComm_Installer.exe`
2. Double-click to run
3. Follow the installation wizard
4. Launch from Start Menu or create desktop shortcut

#### Option 2: Build from Source
1. **Install MinGW** (if not already installed):
   - Download from: https://www.mingw-w64.org/
   - Or use: `choco install mingw-w64` (if using Chocolatey)

2. **Install CMake**:
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

3. **Build the application**:
   ```batch
   build.bat
   ```

4. **Run the application**:
   ```batch
   build\encrypted_comm.exe
   ```

---

### macOS

#### Requirements
- Xcode Command Line Tools
- CMake

#### Installation Steps

1. **Install Xcode Command Line Tools**:
   ```bash
   xcode-select --install
   ```

2. **Install CMake** (if not installed):
   ```bash
   brew install cmake
   ```

3. **Build the application**:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

4. **Run the application**:
   ```bash
   ./build/encrypted_comm
   ```

#### System-wide Installation
```bash
cd build
sudo make install
encrypted_comm  # Run from anywhere
```

---

### Linux (Ubuntu/Debian)

#### Installation Steps

1. **Install required packages**:
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential cmake git
   ```

2. **Build the application**:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

3. **Run the application**:
   ```bash
   ./build/encrypted_comm
   ```

#### System-wide Installation
```bash
cd build
sudo make install
encrypted_comm  # Run from anywhere
```

---

### Linux (Fedora/RHEL/CentOS)

#### Installation Steps

1. **Install required packages**:
   ```bash
   sudo dnf groupinstall "Development Tools"
   sudo dnf install cmake
   ```

2. **Build the application**:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

3. **Run the application**:
   ```bash
   ./build/encrypted_comm
   ```

#### System-wide Installation
```bash
cd build
sudo make install
encrypted_comm  # Run from anywhere
```

---

### FreeBSD

#### Quick Installation (from Source)

##### Using the FreeBSD Build Script (Recommended)
```bash
chmod +x build_freebsd.sh
./build_freebsd.sh
```

##### Using CMake
```bash
sudo pkg install -y cmake gcc13
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=gcc13 -DCMAKE_CXX_COMPILER=g++13 ..
make
sudo make install
```

#### Running the Application
```bash
./build/encrypted_comm  # From build directory
# or
encrypted_comm  # After system-wide install
```

#### Advanced Installation Options

**Using FreeBSD Ports** (if available):
```bash
cd /usr/ports/net-comms/encrypted-comm
sudo make install clean
```

**Using pkg Binary** (if available):
```bash
sudo pkg install encrypted-comm
```

> **Note**: For comprehensive FreeBSD installation guide including ports integration, jails, and troubleshooting, see [FREEBSD_INSTALL.md](FREEBSD_INSTALL.md)

---

## Advanced Build Options

### Using Makefile (Alternative to CMake)

**Unix-like systems only:**

```bash
# Build
make

# Install
sudo make install

# Clean
make clean

# Uninstall
sudo make uninstall
```

### Creating Windows Installer with NSIS

If you want to create the professional installer:

1. **Install NSIS**:
   - Download from: https://nsis.sourceforge.io/
   - Run the installer

2. **Build the application** (see Windows section above)

3. **Create the installer**:
   ```bash
   mkdir installer_temp
   copy build\encrypted_comm.exe installer_temp\
   copy README.md installer_temp\
   
   # Then in NSIS, open installer.nsi and compile
   ```

---

## Portable Installation

### Windows (USB/Portable)

1. Build the application as described above
2. Copy `build\encrypted_comm.exe` to your portable location
3. No installation needed - run directly

### Unix-like (USB/Portable)

1. Build the application as described above
2. Copy `build/encrypted_comm` to your portable location
3. Make executable: `chmod +x encrypted_comm`
4. Run with: `./encrypted_comm`

---

## Troubleshooting Installation

### "CMake not found"
- **Windows**: Download from https://cmake.org/download/, ensure "Add to PATH" is checked
- **macOS**: `brew install cmake`
- **Linux**: `sudo apt-get install cmake` (Ubuntu) or `sudo dnf install cmake` (Fedora)

### "Compiler not found"
- **Windows**: Install MinGW from https://www.mingw-w64.org/
- **macOS**: Run `xcode-select --install`
- **Linux**: `sudo apt-get install build-essential` (Ubuntu) or `sudo dnf groupinstall "Development Tools"` (Fedora)

### "CMake was unable to find a build program corresponding to MinGW Makefiles"
- The Windows `build.bat` now tries multiple strategies automatically:
   - `MinGW Makefiles` (`g++` + `mingw32-make`)
   - `Ninja` (`g++` + `ninja`)
   - CMake default generator (for Visual Studio Build Tools)
- If it still fails, install one complete toolchain:
   - MinGW-w64 (`g++` + `mingw32-make`)
   - Ninja + MinGW-w64 `g++`
   - Visual Studio 2022 Build Tools with **Desktop development with C++**

### "build.sh: Permission denied"
```bash
chmod +x build.sh
./build.sh
```

### Compilation errors
- Delete the `build` directory and try again
- Ensure you have a C++17 compatible compiler
- Check that all dependencies are installed correctly

### Runtime errors on startup
- Ensure the terminal supports ANSI color codes
- Try using a different terminal emulator
- On Windows, use Command Prompt or PowerShell (not Git Bash for colors)

---

## Uninstallation

### Windows
- Use "Add/Remove Programs" or
- Run `uninstall.exe` from the installation directory

### macOS/Linux/FreeBSD
```bash
sudo make uninstall
# or
sudo rm /usr/local/bin/encrypted_comm
```

---

## Getting Help

1. Run the application and type `/help` for in-app assistance
2. Check the README.md for general information
3. Review source code comments for technical details

Enjoy secure communications! 🔐
