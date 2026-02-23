#!/bin/bash
# Build script for Unix-like systems (Linux, macOS, FreeBSD)

set -e

echo "=================================="
echo "Encrypted Communications Builder"
echo "=================================="
echo ""

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "CMake is not installed. Please install CMake first."
    echo ""
    echo "Installation instructions:"
    echo "- macOS: brew install cmake"
    echo "- Ubuntu/Debian: sudo apt-get install cmake"
    echo "- FreeBSD: sudo pkg install cmake"
    exit 1
fi

echo "[1/4] Creating build directory..."
mkdir -p build
cd build

echo "[2/4] Running CMake..."
cmake ..

echo "[3/4] Compiling..."
make

echo "[4/4] Build complete!"
echo ""
echo "=================================="
echo "Build successful!"
echo "=================================="
echo ""
echo "To run the application:"
echo "  ./encrypted_comm"
echo ""
echo "To install system-wide (Unix):"
echo "  sudo make install"
echo ""
