# Build script for C++ project
$ErrorActionPreference = "Stop"

# Get project root
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

# Change to project root
Set-Location $projectRoot

# Remove old build directory
if (Test-Path build) {
    Remove-Item -Recurse build -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 500
}

# Find and setup Ninja
$ninjaPath = (Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\Ninja-build.Ninja_*" -Recurse -ErrorAction SilentlyContinue | Where-Object {$_.Name -eq 'ninja.exe'} | Select-Object -First 1).DirectoryName

# Setup environment
$env:PATH = "C:\Program Files\LLVM\bin;$ninjaPath;C:\Program Files\CMake\bin;$env:PATH"

# Configure with CMake using vcpkg toolchain
Write-Host "Configuring project with CMake..."
& cmake -G 'Ninja' -DCMAKE_CXX_COMPILER='clang++' -DCMAKE_C_COMPILER='clang' -DCMAKE_TOOLCHAIN_FILE='C:\vcpkg\scripts\buildsystems\vcpkg.cmake' -DVCPKG_TARGET_TRIPLET='x64-windows' -S . -B build

# Build
Write-Host "Building project..."
& cmake --build build

Write-Host "Build complete!"
