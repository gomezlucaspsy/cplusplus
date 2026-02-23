# HolyC Version - Encrypted Communications for TempleOS

This directory contains the HolyC implementation of Encrypted Communications for TempleOS.

## Quick Start

### In TempleOS:

```holyc
#include "main.HC"
```

Or to run directly:

```holyc
#include "encryption.HC"
#include "network.HC"
#include "ui.HC"
#include "main.HC"

EncryptedCommMain();
```

## Files

- **`main.HC`** - Main application with menu system and message loop
- **`encryption.HC`** - Password-based encryption module
- **`network.HC`** - Networking and socket handling
- **`ui.HC`** - User interface with retro styling

## Features

✅ End-to-end encryption
✅ Client/Server architecture
✅ Retro terminal UI
✅ TempleOS native implementation
✅ Direct memory management
✅ Full hardware access

## System Requirements

- TempleOS 5.03 or later
- Network support (for networking features)
- Recommended: 512 MB+ RAM

## Usage

1. Start TempleOS
2. Copy the .HC files to your system
3. Include `main.HC` in the terminal
4. Follow the on-screen menu

## For Detailed Information

See [HOLYC_TEMPLEOS_GUIDE.md](../HOLYC_TEMPLEOS_GUIDE.md) for:

- Installation instructions
- Detailed compilation guide
- Usage and commands
- Troubleshooting
- Advanced customization
- Network configuration

## Development Notes

### Design Philosophy
- Uses TempleOS native functions (MAlloc, Free, etc.)
- Leverages built-in data structures (CDblList, CNode)
- Integrates with TempleOS graphics system
- Maintains compatibility with TempleOS 5.03+

### Key Functions

**Encryption:**
- `EncryptionInitialize()` - Initialize with password
- `EncryptionEncrypt()` - Encrypt message
- `EncryptionDecrypt()` - Decrypt message
- `EncryptionCleanup()` - Clean up resources

**Network:**
- `NetworkStartServer()` - Start server mode
- `NetworkConnect()` - Connect as client
- `NetworkSendMessage()` - Send encrypted message
- `NetworkReceiveMessage()` - Receive encrypted message

**UI:**
- `UIInitialize()` - Initialize interface
- `UIDraw()` - Redraw screen
- `UIPrintMessage()` - Display message
- `UIGetInput()` - Get user input
- `UIShowMenu()` - Show main menu

### Extending the Code

To add new features:

```holyc
// Create new module
PUBLIC VOID MyNewFeature() {
    // Feature code
}

// Add to main.HC
#include "myfeature.HC"

// Use in main application
IF (user_chooses_feature) {
    MyNewFeature();
}
```

### Memory Management

Always pair allocations:

```holyc
// Allocate
U8 *buffer = MAlloc(256);

// Use
MemCpy(buffer, source, size);

// Free when done
Free(buffer);
```

### Building Custom Executables

Create a launcher executable:

```holyc
// File: encrypted_comm.HC
PUBLIC VOID EncryptedComm() {
    #include "encryption.HC"
    #include "network.HC"
    #include "ui.HC"
    #include "main.HC"
    EncryptedCommMain();
}

EncryptedComm();
```

Then compile and create executable via TempleOS menu.

## Performance Considerations

1. **Message History** - Limited to 15 messages (configurable)
2. **Buffer Size** - 64KB message limit
3. **Network** - Single connection support
4. **Memory** - Automatically managed by TempleOS

## Known Limitations

1. **Single Connection** - One client at a time (can be extended)
2. **Network API** - Requires TempleOS network stack
3. **Graphics Mode** - Some features work best in graphics mode
4. **File I/O** - Not included in base version

## Troubleshooting

### Won't Compile
- Check file paths are correct
- Ensure all .HC files are in same directory
- Verify TempleOS version 5.03+

### No Network
- Check network drivers loaded
- Test with localhost (127.0.0.1)
- Verify network interface active

### Memory Issues
- Check for memory leaks (Free all MAlloc)
- Increase TempleOS heap if needed
- Monitor with TempleOS memory tools

## Contributing

Improvements welcome! Consider:
- Additional encryption methods
- Multi-client server
- File transfer
- Graphics-based UI
- Command-line interface

## License

Same as parent project - Educational/Open Source

## Resources

- [Main README](../README.md)
- [HolyC/TempleOS Guide](../HOLYC_TEMPLEOS_GUIDE.md)
- [TempleOS Official Site](http://www.templeos.org/)

---

**Enjoy secure communications in TempleOS!** 🖥️
