# Encrypted Communications Application

A cross-platform, retro-styled MS-DOS terminal application for secure encrypted communications.

## Features

- **End-to-End Encryption**: All messages are encrypted using a secure password-based key derivation
- **Cross-Platform**: Works on Windows, macOS, Linux, and FreeBSD
- **Retro MS-DOS Style**: Classic terminal interface with authentic DOS aesthetics
- **Easy Installation**: Simple one-click installation on Windows, easy compilation on Unix-like systems
- **No External Dependencies**: Uses only standard C++ libraries

## Supported Platforms

- Windows (XP and later)
- macOS (10.9+)
- Linux (any distribution)
- FreeBSD (10.0+)
- **TempleOS (5.03+)** - HolyC version available

## Quick Start

### Windows (Easiest)

1. Download and run `encrypted_comm.exe`
2. Follow the on-screen prompts
3. Choose to start as server or connect as client

### macOS/Linux/FreeBSD

#### Using build script:
```bash
chmod +x build.sh
./build.sh
./build/encrypted_comm
```

#### Manual build with CMake:
```bash
mkdir build
cd build
cmake ..
make
./encrypted_comm
```

### TempleOS (HolyC)

1. Copy the HolyC source files from the `holyc/` directory
2. In TempleOS, include the main file:
   ```holyc
   #include "main.HC"
   ```
3. The application will compile and run automatically

See [HOLYC_TEMPLEOS_GUIDE.md](HOLYC_TEMPLEOS_GUIDE.md) for detailed TempleOS instructions.

## Usage

### Starting a Server
1. Run the application
2. Enter a username and encryption password
3. Select option [1] to start as SERVER
4. Share your IP address with the client
5. Wait for incoming connection

### Connecting as Client
1. Run the application
2. Enter the same encryption password as the server
3. Select option [2] to connect as CLIENT
4. Enter server IP address
5. Enter server port (default: 7777)

### Commands

| Command | Description |
|---------|-------------|
| `/help` | Display help information |
| `/status` | Show current connection status |
| `/clear` | Clear message history |
| `/exit` | Exit the application |

## System Requirements

- **Windows**: Windows XP SP3 or later
- **macOS**: OS X 10.9 or later
- **Linux**: glibc 2.17 or compatible
- **FreeBSD**: FreeBSD 10.0 or later
- **OpenBSD/NetBSD/DragonFly BSD**: Compatible versions
- **TempleOS**: Version 5.03 or later with network support

## Documentation

- [Installation Guide](INSTALL.md) - Detailed installation for all platforms
- [FreeBSD Installation Guide](FREEBSD_INSTALL.md) - FreeBSD-specific setup
- [BSD User Guide](BSD_GUIDE.md) - Comprehensive BSD configuration & troubleshooting
- [HolyC/TempleOS Guide](HOLYC_TEMPLEOS_GUIDE.md) - TempleOS development and usage
- [Project Structure](PROJECT_STRUCTURE.md) - Complete project overview and file descriptions

## Building from Source

### Prerequisites

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.12+

### Build Steps

**Windows:**
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

**macOS/Linux/FreeBSD:**
```bash
mkdir build
cd build
cmake ..
make
```

### Installation

After building:

**Windows:**
```bash
cmake --install . --prefix "C:\Program Files\EncryptedComm"
```

**Unix-like:**
```bash
sudo cmake --install . --prefix /usr/local
```

## Creating Windows Installer

To create a `.exe` installer (requires NSIS):

```bash
# (NSIS script would go here)
cpack --config CPackConfig.cmake
```

## Security Notes

- The encryption uses a derived key from your password
- Never share your password with anyone else
- For sensitive communications, use a strong password (12+ characters recommended)
- This is for educational/demonstration purposes. For production critical communication, consider using established cryptography libraries like libsodium or OpenSSL

## Troubleshooting

### "Failed to start server"
- Check if port 7777 is not in use: `netstat -an | grep 7777`
- Try a different port

### "Failed to connect"
- Verify the server IP address is correct
- Check firewall settings (port 7777 must be open)
- Ensure both machines are on the same network

### Color issues on macOS/Linux
- The app uses ANSI color codes which work on most modern terminals
- For best results, use Terminal.app or a compatible terminal emulator

## File Structure

```
encrypted_comm/
├── CMakeLists.txt       # Build configuration
├── README.md            # This file
├── include/             # Header files
│   ├── encryption.h     # Encryption module
│   ├── network.h        # Network/socket module
│   ├── ui.h            # User interface module
│   ├── utils.h         # Utility functions
│   └── config.h        # Configuration constants
├── src/                 # Source files
│   ├── main.cpp        # Main application
│   ├── encryption.cpp
│   ├── network.cpp
│   ├── ui.cpp
│   └── utils.cpp
└── build/              # Build output directory
```

## Version History

- **1.0.0** - Initial release
  - Basic encrypted messaging
  - Server/Client architecture
  - MS-DOS styled UI
  - Cross-platform support

## License

This project is provided as-is for educational purposes.

## Support

For issues or questions, check the help menu in the application (`/help` command).

## Contributing

Contributions are welcome! Feel free to fork and submit pull requests.

---

**Enjoy secure communications with a retro twist!** 🖥️🔐
