# OpenGL Development Environment Setup Script
# Run as administrator

param(
    [string]$VcpkgPath = "C:\vcpkg",
    [string]$MinGWPath = "C:\mingw64"
)

Write-Host "=== OpenGL Development Environment Setup ===" -ForegroundColor Green
Write-Host "This script will install all necessary components for OpenGL application development"
Write-Host ""

# Check administrator privileges
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Error "This script requires administrator privileges. Run PowerShell as administrator."
    exit 1
}

# Function to check if command exists
function Test-CommandExists {
    param($Command)
    try {
        Get-Command $Command -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# Check Git
Write-Host "Checking Git..." -ForegroundColor Yellow
if (-not (Test-CommandExists "git")) {
    Write-Error "Git is not installed. Install Git from https://git-scm.com/download/win"
    exit 1
}
Write-Host "✓ Git found" -ForegroundColor Green

# Check GCC/MinGW
Write-Host "Checking GCC/MinGW..." -ForegroundColor Yellow
if (-not (Test-CommandExists "gcc")) {
    Write-Warning "GCC not found in PATH. Trying to find MinGW..."
    
    # Search popular MinGW paths
    $mingwPaths = @(
        "C:\mingw64\bin",
        "C:\Program Files\mingw-w64\mingw64\bin",
        "C:\ProgramData\mingw64\mingw64\bin",
        "C:\msys64\mingw64\bin"
    )
    
    $mingwFound = $false
    foreach ($path in $mingwPaths) {
        if (Test-Path "$path\gcc.exe") {
            Write-Host "✓ MinGW found at $path" -ForegroundColor Green
            $env:PATH = "$path;$env:PATH"
            $mingwFound = $true
            break
        }
    }
    
    if (-not $mingwFound) {
        Write-Error "MinGW not found. Install MinGW-w64 or add it to PATH"
        Write-Host "Recommended download from: https://www.mingw-w64.org/downloads/"
        exit 1
    }
} else {
    Write-Host "✓ GCC found" -ForegroundColor Green
}

# Install vcpkg
Write-Host "Installing vcpkg..." -ForegroundColor Yellow
if (Test-Path $VcpkgPath) {
    Write-Host "vcpkg already exists at $VcpkgPath" -ForegroundColor Yellow
    $response = Read-Host "Reinstall vcpkg? (y/N)"
    if ($response -eq "y" -or $response -eq "Y") {
        Remove-Item -Recurse -Force $VcpkgPath
    } else {
        Write-Host "Skipping vcpkg installation" -ForegroundColor Yellow
        $skipVcpkg = $true
    }
}

if (-not $skipVcpkg) {
    try {
        Write-Host "Cloning vcpkg..." -ForegroundColor Yellow
        git clone https://github.com/Microsoft/vcpkg.git $VcpkgPath
        
        Write-Host "Initializing vcpkg..." -ForegroundColor Yellow
        Set-Location $VcpkgPath
        .\bootstrap-vcpkg.bat
        
        Write-Host "✓ vcpkg successfully installed" -ForegroundColor Green
    } catch {
        Write-Error "Error installing vcpkg: $($_.Exception.Message)"
        exit 1
    }
}

# Install GLFW via vcpkg
Write-Host "Installing GLFW3..." -ForegroundColor Yellow
try {
    Set-Location $VcpkgPath
    .\vcpkg install glfw3:x64-windows
    Write-Host "✓ GLFW3 successfully installed" -ForegroundColor Green
} catch {
    Write-Error "Error installing GLFW3: $($_.Exception.Message)"
    exit 1
}

# Update PATH variable
Write-Host "Updating PATH variable..." -ForegroundColor Yellow
$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
$vcpkgBinPath = "$VcpkgPath"

if ($currentPath -notlike "*$vcpkgBinPath*") {
    $newPath = "$currentPath;$vcpkgBinPath"
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "Machine")
    Write-Host "✓ PATH updated" -ForegroundColor Green
} else {
    Write-Host "✓ vcpkg already in PATH" -ForegroundColor Green
}

# Create example CMakeLists.txt
Write-Host "Creating example CMakeLists.txt..." -ForegroundColor Yellow
$cmakeContent = @"
cmake_minimum_required(VERSION 3.20)
project(OpenGLExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLFW via vcpkg
find_package(glfw3 CONFIG REQUIRED)

# Add GLAD (assumes GLAD files are in glad/ folder)
add_library(glad glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# Create executable
add_executable(hellowindow2 hellowindow2.cpp)

# Link libraries
target_link_libraries(hellowindow2 PRIVATE glfw glad)

# Copy DLL for runtime
if(WIN32)
    add_custom_command(TARGET hellowindow2 POST_BUILD
        COMMAND `${CMAKE_COMMAND} -E copy_if_different
        "$VcpkgPath/installed/x64-windows/bin/glfw3.dll"
        `$<TARGET_FILE_DIR:hellowindow2>)
endif()
"@

# Save to current directory if we're not in vcpkg
if ((Get-Location).Path -eq $VcpkgPath) {
    Set-Location $PSScriptRoot
}

$cmakeContent | Out-File -FilePath "CMakeLists_example.txt" -Encoding UTF8
Write-Host "✓ Example CMakeLists.txt created (CMakeLists_example.txt)" -ForegroundColor Green

# Create batch file for compilation
Write-Host "Creating batch file for compilation..." -ForegroundColor Yellow
$batchContent = @"
@echo off
echo Compiling OpenGL application...

REM Check for source file
if not exist hellowindow2.cpp (
    echo Error: hellowindow2.cpp file not found!
    pause
    exit /b 1
)

REM Check for GLAD files
if not exist glad\include\glad\gl.h (
    echo Error: GLAD files not found! Make sure glad\ folder exists.
    pause
    exit /b 1
)

REM Compilation
echo Compiling...
g++ hellowindow2.cpp -Iglad/include -I"$VcpkgPath/installed/x64-windows/include" glad/src/gl.c -L"$VcpkgPath/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

if %ERRORLEVEL% NEQ 0 (
    echo Compilation error!
    pause
    exit /b 1
)

REM Copy DLL
echo Copying required DLLs...
copy "$VcpkgPath\installed\x64-windows\bin\glfw3.dll" . >nul 2>&1

echo Compilation completed successfully!
echo Running program...
hellowindow2.exe

pause
"@

$batchContent | Out-File -FilePath "compile_opengl.bat" -Encoding ASCII
Write-Host "✓ Batch file created (compile_opengl.bat)" -ForegroundColor Green

Write-Host ""
Write-Host "=== Installation completed! ===" -ForegroundColor Green
Write-Host ""
Write-Host "What's next:" -ForegroundColor Cyan
Write-Host "1. Restart command prompt to update PATH"
Write-Host "2. Make sure you have GLAD files in glad/ folder"
Write-Host "3. For CMake compilation use:"
Write-Host "   cmake -S . -B build -G `"MinGW Makefiles`" -DCMAKE_TOOLCHAIN_FILE=$VcpkgPath/scripts/buildsystems/vcpkg.cmake"
Write-Host "   cmake --build build"
Write-Host "4. For direct compilation use: compile_opengl.bat"
Write-Host ""
Write-Host "Documentation: SETUP_GUIDE.md" -ForegroundColor Yellow