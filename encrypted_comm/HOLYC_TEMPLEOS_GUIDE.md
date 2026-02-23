# HolyC/TempleOS Support for Encrypted Communications

## Overview

This document provides complete information about running Encrypted Communications on TempleOS using HolyC.

## Table of Contents

1. [What is TempleOS and HolyC?](#what-is-templeos-and-holyc)
2. [Supported Versions](#supported-versions)
3. [Installation & Setup](#installation--setup)
4. [Building from Source](#building-from-source)
5. [Usage Guide](#usage-guide)
6. [HolyC Specific Features](#holyc-specific-features)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Topics](#advanced-topics)

---

## What is TempleOS and HolyC?

### TempleOS
**TempleOS** is a 64-bit, open-source, ring-0-only operating system created by Terry Davis. It's designed to be simple, fast, and useful, with a unique combination of features:

- **Single Address Space**: All code and data share one address space
- **Ring-0 Only**: No user/kernel privilege separation
- **Native Code Generation**: JIT compilation of code at runtime
- **Direct Hardware Access**: Low-level hardware manipulation
- **Built-in Development Environment**: Includes compiler and tools

### HolyC
**HolyC** is the primary programming language for TempleOS. It's based on C but includes extensions for TempleOS's unique architecture:

- **C-like Syntax**: Familiar to C/C++ programmers
- **Dynamic Compilation**: Code can be compiled and executed at runtime
- **Direct Hardware Access**: Macros and functions for low-level operations
- **Graphics Support**: Built-in graphics and UI capabilities
- **String Features**: U8* and other conveniences
- **Automatic Memory Management**: Some memory management features

---

## Supported Versions

- **TempleOS**: 5.03+ (Supported)
- **TempleOS Hobby Edition**: Compatible
- **TempleOS Public Domain**: Compatible

**Compatibility Note**: The Encrypted Communications application is designed for modern TempleOS versions with network capabilities.

---

## Installation & Setup

### System Requirements

1. **TempleOS Installation**
   - TempleOS 5.03 or later installed and running
   - Minimum 512 MB RAM
   - Network interface (for networking features)

2. **File System Access**
   - Access to TempleOS file system
   - Ability to compile HolyC programs

### Downloading the Application

```bash
# In TempleOS, download the source files
# Place in your application directory, e.g.: /Home/**
#
# Files needed:
# - encryption.HC
# - network.HC
# - ui.HC
# - main.HC
```

---

## Building from Source

### Method 1: Using TempleOS IDE (Recommended)

1. **Open the IDE**:
   - Press `Ctrl+Home` to access the TempleOS menu
   - Navigate to editor

2. **Load the main file**:
   - File → Open
   - Select `main.HC`

3. **Compile and Run**:
   - Press `F5` (or Ctrl+T) to compile and execute
   - The application will start immediately

### Method 2: Command Line Compile

```bash
# In TempleOS terminal or program
#include "main.HC"
```

### Method 3: Using a Script

Create a compile script (e.g., `compile.HC`):

```holyc
/**
 * Compilation script for Encrypted Communications
 */

"\f";  // Clear screen

"Compiling Encrypted Communications...\n";

#include "encryption.HC"
#include "network.HC"
#include "ui.HC"
#include "main.HC"

"Compilation complete!\n";
"Run with: EncryptedCommMain();\n";
```

Then run from TempleOS:

```bash
#include "compile.HC"
```

---

## Usage Guide

### Starting the Application

In TempleOS:

```holyc
EncryptedCommMain();
```

Or if using automatic execution:

```holyc
#include "main.HC"
```

### Main Menu

Upon starting, you'll see:

```
════════════════════════════════════════════════════════════════════════════════
               ENCRYPTED COMM v1.0 - TempleOS Edition
════════════════════════════════════════════════════════════════════════════════

  [1] Start as SERVER (Listen for connections)
  [2] Connect as CLIENT
  [3] Exit

Choose option [1-3]: 
```

### Running as Server

1. Select option `1`
2. The application will start a server on port 7777
3. Share the IP with clients (default: 127.0.0.1 for local)
4. Wait for incoming connections

### Running as Client

1. Select option `2`
2. Enter server IP address
3. Enter server port (default: 7777)
4. Wait for connection to establish

### Messaging

Once connected:

- Type your message and press ENTER to send
- Messages appear with timestamps and sender info
- Use commands with the `/` prefix

### Available Commands

| Command | Description |
|---------|-------------|
| `/help` | Display help information |
| `/status` | Show connection status |
| `/clear` | Clear message history |
| `/exit` | Exit the application |

---

## HolyC Specific Features

### Memory Management

The HolyC version uses TempleOS memory functions:

```holyc
MAlloc()      // Allocate memory
Free()        // Free memory
CAlloc()      // Allocate and clear memory
MemCpy()      // Copy memory
MemSet()      // Set memory
```

### String Handling

```holyc
U8 *str = "Hello";     // String pointer
StrLen(str)            // String length
StrCmp(str1, str2)     // Compare strings
SPrintF(buf, fmt, ...) // Format string
Str2I64(str)           // String to integer
```

### Input/Output

```holyc
GetStr(buffer, size)   // Get user input
// Printing is done with implicit string conversion
```

### Graphics and UI

The HolyC version can take advantage of TempleOS graphics:

```holyc
'\f'                   // Clear screen
WHITE, CYAN, GREEN, etc. // Color constants
GrPrint()              // Graphics print (if using graphics mode)
```

### Data Structures

```holyc
CDblList               // Doubly-linked list header
CNode                  // List node structure
InsQue(node, qbase)    // Insert in queue
RemQue(node)           // Remove from queue
```

---

## Troubleshooting

### Compilation Errors

#### Error: "Unknown identifier"
- **Cause**: Missing #include files
- **Solution**: Ensure all .HC files are in the same directory and properly included

#### Error: "Memory allocation failed"
- **Cause**: Not enough memory
- **Solution**: Free unused data or increase TempleOS heap size

#### Error: "Network initialization failed"
- **Cause**: Network not available in TempleOS
- **Solution**: Ensure TempleOS has network support enabled

### Runtime Issues

#### Application Won't Start
```holyc
// Test basic functionality
UIPrintColored("Test", WHITE);
```

#### No Network Connection
- Check firewall settings (if applicable)
- Verify network interface is active
- Test with localhost (127.0.0.1) first

#### Memory Leaks
- Use `Free()` for every `MAlloc()`
- Check message history cleanup
- Monitor with TempleOS memory tools

---

## Advanced Topics

### Custom Build Configuration

You can customize the application by modifying constants:

```holyc
// In encryption.HC
#define KEY_SIZE 32          // Encryption key size
#define NONCE_SIZE 24        // Nonce size
#define MAX_MESSAGE_SIZE 65536

// In ui.HC
#define MAX_MESSAGE_HISTORY 15  // Messages to keep
```

### Network Configuration

Modify network defaults:

```holyc
#define DEFAULT_PORT 7777
#define MAX_CONNECTIONS 10
```

### Extending the Application

Add new features by:

1. Creating new .HC files
2. Including them in main.HC
3. Calling functions or adding menu options

Example:

```holyc
// Create encryption_advanced.HC with new functions
#include "encryption_advanced.HC"

// Use in main:
BOOL result = EncryptionAdvancedInit();
```

### Debugging

Enable debug output:

```holyc
#define DEBUG 1

// Then use:
IF (DEBUG) {
    UIPrintColored("Debug info...\n", WHITE);
}
```

### Performance Optimization

1. **Minimize allocations**: Pre-allocate buffers when possible
2. **Use efficient structures**: CDblList is efficient for message history
3. **Avoid repeated copying**: Reuse buffers
4. **Profile with TempleOS tools**: Use profiler to find bottlenecks

---

## Integration with TempleOS

### Add to Autostart

Create a startup file:

```holyc
// File: /Home/bin/enc_comm.HC
#include "/Home/src/encrypted_comm/main.HC"
```

Then reference in startup scripts.

### Creating a Program Launcher

```holyc
PUBLIC VOID EncCommLauncher() {
    // Pre-test
    IF (!EncryptionInitialize("test")) {
        "Failed to initialize\n";
        return;
    }
    
    // Start application
    EncryptedCommMain();
}
```

### System Integration Notes

- TempleOS is ring-0 only, so all code runs with full privileges
- Network functionality depends on TempleOS TCP/IP support
- Memory is automatically managed by TempleOS in many cases
- The `\f` character clears the screen (form feed)

---

## TempleOS-Specific Advantages

The HolyC version benefits from TempleOS's unique features:

1. **Direct Hardware Access**
   - No kernel boundary crossings
   - Direct network interface control

2. **Unified Address Space**
   - No context switching overhead
   - Simplified memory management

3. **Dynamic Compilation**
   - Code can be modified and compiled at runtime
   - No separate compilation step needed

4. **Built-in IDE**
   - Integrated development environment
   - Built-in debugger and profiler

---

## Limitations and Notes

1. **Network Support**: Requires TempleOS to have network drivers loaded
2. **Security**: TempleOS ring-0 model means all code has hardware access
3. **Compatibility**: Limited to TempleOS systems only
4. **Graphics**: Advanced UI may require graphics mode
5. **Performance**: TempleOS is optimized for simplicity, not maximum performance

---

## Additional Resources

- **TempleOS Official**: http://www.templeos.org/
- **TempleOS Documentation**: Built-in help in TempleOS
- **HolyC Documentation**: `/Doc/` directory in TempleOS Distro
- **TempleOS Community**: Forums and community resources online

---

## Support and Contributions

For TempleOS-specific improvements:

1. Test on supported TempleOS versions
2. Document platform-specific fixes
3. Share improvements with the community
4. Report bugs with TempleOS version info

---

**Note**: This HolyC implementation is adapted for TempleOS's unique architecture. Some network features may require additional configuration based on your TempleOS installation's capabilities.

---

**Enjoy secure communications in the unique TempleOS environment!** 🖥️🔐
