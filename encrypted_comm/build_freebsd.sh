#!/bin/sh
# FreeBSD Build and Install Script

set -e

echo "=================================="
echo "FreeBSD Build Configuration"
echo "=================================="
echo ""

# Check OS
if [ "$(uname -s)" != "FreeBSD" ]; then
    echo "This script is designed for FreeBSD."
    echo "For other systems, use build.sh instead."
    exit 1
fi

# Check if required tools are installed
if ! command -v cmake > /dev/null 2>&1; then
    echo "CMake is not installed."
    echo "Install with: sudo pkg install cmake"
    exit 1
fi

echo "[1/5] Updating package database..."
sudo pkg update -q

echo "[2/5] Installing dependencies (if needed)..."
sudo pkg install -y cmake gcc

echo "[3/5] Creating build directory..."
mkdir -p build
cd build

echo "[4/5] Running CMake with FreeBSD settings..."
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..

echo "[5/5] Compiling..."
make

echo ""
echo "=================================="
echo "Build successful!"
echo "=================================="
echo ""
echo "To run the application:"
echo "  ./encrypted_comm"
echo ""
echo "To install system-wide:"
echo "  sudo make install"
echo ""
