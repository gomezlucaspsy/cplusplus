# FreeBSD Installation Guide

## Quick Installation Methods

### Method 1: Using FreeBSD Ports (Recommended for Port Users)

If the port has been added to the FreeBSD Ports Collection:

```bash
# Using portmaster
sudo portmaster net-comms/encrypted-comm

# Or using the traditional method
cd /usr/ports/net-comms/encrypted-comm
sudo make install clean

# Or using Poudriere (if you use it)
poudriere bulk -j freebsd -p ports encrypted-comm
```

### Method 2: Using pkg (Binary Package)

```bash
# If package is available
sudo pkg install encrypted-comm
```

### Method 3: Building from Source

#### Requirements

```bash
# Install required packages
sudo pkg install -y cmake gcc13
```

#### Build Steps

**Option A: Using the FreeBSD build script (recommended)**
```bash
chmod +x build_freebsd.sh
./build_freebsd.sh
```

**Option B: Using CMake directly**
```bash
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=gcc13 -DCMAKE_CXX_COMPILER=g++13 ..
make
sudo make install
```

**Option C: Using Makefile**
```bash
make
sudo make install
```

---

## FreeBSD Jail/Container Installation

### Using FreeBSD Jails

1. **Within the jail:**
```bash
pkg install cmake gcc13
git clone <repo_url>
cd encrypted_comm
./build_freebsd.sh
sudo make install
```

### Using OCI Containers (if using Podman/Docker)

```dockerfile
FROM freebsd:13.2

RUN pkg update && \
    pkg install -y cmake gcc13 git

WORKDIR /opt
RUN git clone <repo_url>
WORKDIR /opt/encrypted_comm
RUN chmod +x build_freebsd.sh && ./build_freebsd.sh
RUN make install

ENTRYPOINT ["encrypted_comm"]
```

---

## Troubleshooting FreeBSD Installation

### Compiler Issues

If you encounter `g++` not found:
```bash
# Use the explicit version
update-alternatives --config c++
# Or specify when building
cmake -DCMAKE_CXX_COMPILER=g++13 ..
```

### CMake Not Found

```bash
sudo pkg install cmake
```

### Permission Denied on Scripts

```bash
chmod +x build_freebsd.sh
chmod +x build.sh
```

### Library Issues

If you get library not found errors:
```bash
# Rebuild with verbose output
make VERBOSE=1

# Or check what's missing
ldd ./build/encrypted_comm
```

---

## Installing to Custom Location

### System-wide Installation
```bash
cd build
sudo make install
# Binary installed to: /usr/local/bin/encrypted_comm
```

### User Installation
```bash
cd build
make install DESTDIR=$HOME/.local/
```

### For Ports Integration

Create `/usr/ports/net-comms/encrypted-comm/` directory with appropriate `Makefile`, `distinfo`, and `pkg-descr`.

---

## Post-Installation

### Creating a Service/Daemon (Optional)

Create `/usr/local/etc/rc.d/encrypted_comm`:

```bash
#!/bin/sh
#
# PROVIDE: encrypted_comm
# REQUIRE: networking
# KEYWORD: shutdown
#
# encrypted_comm_enable (bool): Set to YES to enable service
#

. /etc/rc.subr

name="encrypted_comm"
rcvar="${name}_enable"
pidfile="/var/run/${name}.pid"
command="/usr/local/bin/${name}"

load_rc_config $name
: ${encrypted_comm_enable:=NO}

run_rc_command "$1"
```

Make it executable:
```bash
sudo chmod +x /usr/local/etc/rc.d/encrypted_comm
```

Enable and start:
```bash
sudo sysrc encrypted_comm_enable="YES"
sudo service encrypted_comm start
```

---

## Uninstallation

### If installed via pkg:
```bash
sudo pkg delete encrypted-comm
```

### If installed from source:
```bash
sudo make uninstall
```

### If installed via ports:
```bash
sudo portmaster -e net-comms/encrypted-comm
# or
cd /usr/ports/net-comms/encrypted-comm && sudo make deinstall
```

### Manual removal:
```bash
sudo rm /usr/local/bin/encrypted_comm
```

---

## FreeBSD-Specific Notes

1. **Shell**: FreeBSD uses `/bin/sh` by default. Scripts are compatible.
2. **Compiler**: FreeBSD may use alternative compilers (gcc13, clang). Specify with `CMAKE_CXX_COMPILER`.
3. **Port Ownership**: Consider creating a non-root user for running services.
4. **Firewall**: If using `pf` firewall, ensure port 7777 is open:
   ```
   pass in proto tcp from any to any port 7777
   ```
5. **Performance**: FreeBSD's network stack is highly optimized for communications applications.

---

## Creating a FreeBSD Port

To contribute this to FreeBSD Ports:

1. Create the port directory:
```bash
mkdir -p /usr/ports/net-comms/encrypted-comm
```

2. Add required files (see FreeBSD Porters Handbook)

3. Generate checksums:
```bash
cd /usr/ports/net-comms/encrypted-comm
make makesum
```

4. Test the port:
```bash
sudo make install clean
sudo make deinstall
```

5. Submit to FreeBSD Ports for review

---

For more FreeBSD-specific information, see:
- [FreeBSD Handbook](https://docs.freebsd.org/)
- [FreeBSD Ports Guide](https://docs.freebsd.org/en/books/porters-handbook/)
- [FreeBSD pkg Documentation](https://man.freebsd.org/pkg)
