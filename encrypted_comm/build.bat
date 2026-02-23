@echo off
setlocal EnableExtensions
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

REM Add common MinGW paths if available but not in PATH
if exist C:\MinGW\bin\g++.exe set "PATH=C:\MinGW\bin;%PATH%"
if exist C:\msys64\mingw64\bin\g++.exe set "PATH=C:\msys64\mingw64\bin;%PATH%"
if exist C:\msys64\ucrt64\bin\g++.exe set "PATH=C:\msys64\ucrt64\bin;%PATH%"

echo [1/4] Detecting build tools...
set "GENERATOR="

echo [2/4] Creating build directory...
if not exist build mkdir build
cd build

echo [3/4] Running CMake...

where mingw32-make >nul 2>nul
if %errorlevel% equ 0 (
    where g++ >nul 2>nul
    if %errorlevel% equ 0 (
        call :clean_cache
        echo Trying generator: MinGW Makefiles
        cmake -S .. -B . -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
        if %errorlevel% equ 0 (
            set "GENERATOR=MinGW Makefiles"
            goto :configured
        )
    )
)

where ninja >nul 2>nul
if %errorlevel% equ 0 (
    where g++ >nul 2>nul
    if %errorlevel% equ 0 (
        call :clean_cache
        echo Trying generator: Ninja
        cmake -S .. -B . -G "Ninja" -DCMAKE_CXX_COMPILER=g++
        if %errorlevel% equ 0 (
            set "GENERATOR=Ninja"
            goto :configured
        )
    )
)

call :clean_cache
echo Trying CMake default generator...
cmake -S .. -B .
if %errorlevel% equ 0 (
    set "GENERATOR=Default"
    goto :configured
)

color 0C
echo CMake configuration failed with all supported strategies.
echo.
echo Install one of the following toolchains, then re-run build.bat:
echo   - MinGW-w64 ^(g++ + mingw32-make^)
echo   - Ninja + g++
echo   - Visual Studio 2022 Build Tools ^(Desktop development with C++^)
echo.
echo If MinGW is already installed, add its bin folder to PATH.
pause
exit /b 1

:configured
echo Selected generator: %GENERATOR%

echo [4/4] Compiling...
cmake --build . --config Release
if %errorlevel% neq 0 (
    color 0C
    echo Compilation failed!
    pause
    exit /b 1
)

echo Build complete!
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
exit /b 0

:clean_cache
if exist CMakeCache.txt del /f /q CMakeCache.txt >nul 2>nul
if exist CMakeFiles rmdir /s /q CMakeFiles >nul 2>nul
exit /b 0
