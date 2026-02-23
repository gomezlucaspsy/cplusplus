@echo off
REM Build script for Windows

color 0A
cls

echo ==================================
echo Encrypted Communications Builder
echo ==================================
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    color 0C
    echo CMake is not installed or not in PATH.
    echo.
    echo Please download and install CMake from: https://cmake.org/download/
    echo.
    echo Make sure to add CMake to your PATH during installation.
    pause
    exit /b 1
)

echo [1/4] Creating build directory...
if not exist build mkdir build
cd build

echo [2/4] Running CMake...
cmake -G "MinGW Makefiles" ..
if %errorlevel% neq 0 (
    color 0C
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo [3/4] Compiling...
mingw32-make
if %errorlevel% neq 0 (
    color 0C
    echo Compilation failed!
    pause
    exit /b 1
)

echo [4/4] Build complete!
echo.

color 0B
echo ==================================
echo Build successful!
echo ==================================
echo.
echo To run the application:
echo   encrypted_comm.exe
echo.
color 0A

pause
