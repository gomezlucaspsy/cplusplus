# BSD Software Installation and Development Guide

This document provides comprehensive information about running Encrypted Communications on BSD systems.

## Table of Contents

1. [Supported BSD Systems](#supported-bsd-systems)
2. [Quick Start](#quick-start)
3. [Installation Methods](#installation-methods)
4. [Advanced Configuration](#advanced-configuration)
5. [Troubleshooting](#troubleshooting)

---

## Supported BSD Systems

- **FreeBSD** (10.0+) - Primary BSD support
- **OpenBSD** (6.0+) - Compatible
- **NetBSD** (7.0+) - Compatible
- **DragonFly BSD** (5.0+) - Compatible

---

## Quick Start

### FreeBSD (Easiest)

```bash
# Install dependencies
sudo pkg install -y cmake gcc13

# Clone/Download sources
cd encrypted_comm

# Build using our BSD script
chmod +x build_freebsd.sh
./build_freebsd.sh

# Run
./build/encrypted_comm
```

---

## Installation Methods

### Method 1: Using Build Script (Recommended)

**For all BSD variants:**
```bash
chmod +x build_freebsd.sh
./build_freebsd.sh
```

This script:
- Detects your OS (validates it's FreeBSD)
- Installs dependencies via `pkg`
- Configures CMake with BSD-specific settings
- Compiles the application
- Provides installation instructions

### Method 2: Manual CMake Build

```bash
# Install compiler and CMake
sudo pkg install cmake gcc13

# Create and enter build directory
mkdir build && cd build

# Configure with BSD compiler
cmake -DCMAKE_C_COMPILER=gcc13 \
      -DCMAKE_CXX_COMPILER=g++13 \
      -DCMAKE_CXX_FLAGS="-std=c++17" \
      ..

# Build
make -j4

# Install system-wide
sudo make install
```

### Method 3: Using Makefile

```bash
# Build
make

# Install
sudo make install PREFIX=/usr/local

# Verify
which encrypted_comm
```

### Method 4: From FreeBSD Ports (If Added)

```bash
# Using portmaster
sudo portmaster net-comms/encrypted-comm

# Or traditional method
cd /usr/ports/net-comms/encrypted-comm
sudo make install clean
```

### Method 5: Using pkg Binary (If Available)

```bash
sudo pkg install encrypted-comm
```

---

## Advanced Configuration

### Custom Installation Path

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/encrypted-comm \
      -DCMAKE_C_COMPILER=gcc13 \
      -DCMAKE_CXX_COMPILER=g++13 ..
make
sudo make install
```

### Building for Specific BSD Variant

**OpenBSD:**
```bash
doas pkg_add cmake gcc%13
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=eg++ ..
make
```

**NetBSD:**
```bash
sudo pkgin install cmake gcc13
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=g++13 ..
make
```

**DragonFly BSD:**
```bash
sudo pkg install cmake gcc13
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=g++13 ..
make
```

### Enabling Optimizations

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
make
```

### Static Linking (for portability)

```bash
cmake -DCMAKE_CXX_FLAGS="-static-libstdc++ -static-libgcc" ..
make
```

---

## Creating a FreeBSD Service

### Step 1: Create RC Script

Create `/usr/local/etc/rc.d/encrypted_comm`:

```sh
#!/bin/sh
#
# PROVIDE: encrypted_comm
# REQUIRE: networking
# KEYWORD: shutdown
#

. /etc/rc.subr

name="encrypted_comm"
rcvar="${name}_enable"
pidfile="/var/run/${name}.pid"
command="/usr/local/bin/${name}"
command_args=">> /var/log/${name}.log 2>&1"
user="nobody"
group="nogroup"

load_rc_config $name
: ${encrypted_comm_enable:=NO}

run_rc_command "$1"
```

### Step 2: Enable and Start

```bash
# Make executable
sudo chmod +x /usr/local/etc/rc.d/encrypted_comm

# Enable at boot
echo 'encrypted_comm_enable="YES"' | sudo tee -a /etc/rc.conf

# Start service
sudo service encrypted_comm start

# Check status
sudo service encrypted_comm status
```

---

## Using with Jails

### FreeBSD Jail Example

```bash
# Create jail
sudo iocage create -r 13.2-RELEASE -n encrypted-app

# Install into jail
sudo iocage console encrypted-app
pkg install -y cmake gcc13

# Clone source
git clone <repo> encrypted_comm
cd encrypted_comm
./build_freebsd.sh

# Exit jail
exit

# Start jail
sudo iocage start encrypted-app

# Connect to jail
sudo iocage console encrypted-app
encrypted_comm
```

---

## Performance Tuning (FreeBSD)

### Network Buffer Settings

Edit `/etc/sysctl.conf`:

```conf
# Increase network buffers for better throughput
net.inet.tcp.recvspace=65536
net.inet.tcp.sendspace=65536

# Enable TCP Window Scaling
net.inet.tcp.rfc1323=1

# Optimize for low latency
net.inet.tcp.delayed_ack=0
```

Apply settings:
```bash
sudo sysctl -p
```

### Firewall Configuration (pf)

Edit `/etc/pf.conf`:

```
# Allow encrypted comm on port 7777
pass in proto tcp from any to any port 7777

# Apply settings
sudo pfctl -f /etc/pf.conf
```

---

## Troubleshooting

### Common Issues

#### 1. CMake Not Found
```bash
# FreeBSD
sudo pkg install cmake

# OpenBSD
doas pkg_add cmake

# NetBSD
sudo pkgin install cmake
```

#### 2. Compiler Not Found
```bash
# FreeBSD - Install GCC 13
sudo pkg install gcc13

# Then use it explicitly
cmake -DCMAKE_CXX_COMPILER=g++13 ..
```

#### 3. Permission Denied
```bash
# Make scripts executable
chmod +x build_freebsd.sh

# Use sudo for installation
sudo make install
```

#### 4. Port Already in Use
```bash
# Find what's using port 7777
sudo netstat -tlnp | grep 7777

# Or use lsof
sudo lsof -i :7777

# Change port in code or firewall settings
```

#### 5. Build Fails
```bash
# Clean build directory
cd build
make distclean
cd ..
rm -rf build

# Try again with verbose output
mkdir build && cd build
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

### Diagnostic Commands

```bash
# Check FreeBSD version
freebsd-version

# Check installed compilers
gcc --version
g++13 --version

# Verify CMake
cmake --version

# Check make
make --version

# Test network
$ netstat -an | grep 7777
```

---

## Uninstallation

### If Built from Source
```bash
cd build
sudo make uninstall
```

### If Using Ports
```bash
sudo portmaster -e encrypted-comm
# or
cd /usr/ports/net-comms/encrypted-comm
sudo make deinstall
```

### If Using pkg
```bash
sudo pkg delete encrypted-comm
```

### Manual Removal
```bash
sudo rm /usr/local/bin/encrypted_comm
```

---

## Additional Resources

- [FreeBSD Handbook](https://docs.freebsd.org/en/books/handbook/)
- [FreeBSD Porter's Handbook](https://docs.freebsd.org/en/books/porters-handbook/)
- [CMake Documentation](https://cmake.org/documentation/)
- [OpenBSD FAQ](https://www.openbsd.org/faq/)
- [NetBSD Wiki](https://wiki.netbsd.org/)

---

## Contributing BSD Improvements

To improve BSD support:
1. Test on your BSD variant
2. Report any issues with specific build configurations
3. Submit patches for OS-specific optimizations
4. Help create official ports

---

For general installation help, see [INSTALL.md](INSTALL.md)
