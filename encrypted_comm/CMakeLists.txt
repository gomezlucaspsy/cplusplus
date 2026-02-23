cmake_minimum_required(VERSION 3.12)
project(EncryptedComm VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(PLATFORM_LIBS ws2_32 crypt32)
elseif(APPLE)
    set(PLATFORM_LIBS "")
    add_definitions(-DMACOS)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(PLATFORM_LIBS "")
    add_definitions(-DFREEBSD)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    set(PLATFORM_LIBS "")
    add_definitions(-DOPENBSD)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    set(PLATFORM_LIBS "")
    add_definitions(-DNETBSD)
elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly")
    set(PLATFORM_LIBS "")
    add_definitions(-DDRAGONFLY)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
else()
    # Generic Unix-like
    set(PLATFORM_LIBS "")
    add_definitions(-DUNIX)
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/encryption.cpp
    src/network.cpp
    src/ui.cpp
    src/utils.cpp
)

set(HEADERS
    include/encryption.h
    include/network.h
    include/ui.h
    include/utils.h
    include/config.h
)

# Create executable
add_executable(encrypted_comm ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(encrypted_comm PRIVATE ${PLATFORM_LIBS})

# Platform-specific post-build
if(WIN32)
    set_target_properties(encrypted_comm PROPERTIES
        WIN32_EXECUTABLE OFF
    )
endif()

# Installation
install(TARGETS encrypted_comm DESTINATION bin)
install(FILES README.md DESTINATION .)
