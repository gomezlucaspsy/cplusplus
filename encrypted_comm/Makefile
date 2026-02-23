# Makefile for Encrypted Communications

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS := -pthread

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lm
endif
ifeq ($(UNAME_S),Darwin)
    # macOS
endif

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET := $(BIN_DIR)/encrypted_comm

# Default target
.PHONY: all
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Build complete!"
	@echo "Run with: ./$(TARGET)"

# Clean build directory
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Install (Unix-like systems)
.PHONY: install
install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/encrypted_comm
	@echo "Installation complete! Run with: encrypted_comm"

# Uninstall
.PHONY: uninstall
uninstall:
	rm -f /usr/local/bin/encrypted_comm
	@echo "Uninstall complete!"

# Help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all       - Build the application (default)"
	@echo "  clean     - Remove build directory"
	@echo "  install   - Install system-wide (requires sudo)"
	@echo "  uninstall - Remove installed binary"
	@echo "  help      - Show this help message"
