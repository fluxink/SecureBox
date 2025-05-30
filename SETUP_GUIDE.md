# OpenGL + GLFW + GLAD Setup Guide

This guide will help you set up a development environment for OpenGL with GLFW and GLAD on Windows.

## Requirements

- Windows 10/11
- MinGW-w64 (GCC)
- Git
- CMake (optional)
- Visual Studio Code (optional)

## Quick Automatic Installation

Run PowerShell as Administrator and execute:

```powershell
.\setup_opengl_environment.ps1
```

## Manual Installation

### 1. vcpkg Installation

```powershell
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Initialize vcpkg
.\bootstrap-vcpkg.bat

# Install GLFW
.\vcpkg install glfw3:x64-windows
```

### 2. MinGW-w64 Installation (if not installed)

Download and install MinGW-w64 from the official website or through a package manager.

### 3. Environment Variables Setup

Add to PATH:
- Path to MinGW-w64 bin (e.g.: `C:\mingw64\bin`)
- Path to vcpkg (e.g.: `C:\vcpkg`)

## Project Compilation

### Method 1: CMake (recommended)

```powershell
# Project setup
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build
cmake --build build

# Run
.\build\hellowindow2.exe
```

### Method 2: Direct compilation

```powershell
# Compilation
g++ hellowindow2.cpp -Iglad/include -I"C:\vcpkg\installed\x64-windows\include" glad/src/gl.c -L"C:\vcpkg\installed\x64-windows\lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

# Copy DLL
copy "C:\vcpkg\installed\x64-windows\bin\glfw3.dll" .

# Run
.\hellowindow2.exe
```

### Method 3: Using batch file

```cmd
compile_opengl.bat
```

## VS Code Setup

For correct IntelliSense operation in VS Code, make sure the correct paths are specified in `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/glad/include",
                "C:/vcpkg/installed/x64-windows/include"
            ],
            "compilerPath": "C:/mingw64/bin/gcc.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

## Project structure

```
project/
├── hellowindow2.cpp          # Main source file
├── CMakeLists.txt            # CMake configuration
├── glad/                     # GLAD library
│   ├── include/
│   │   ├── glad/gl.h
│   │   └── KHR/khrplatform.h
│   └── src/gl.c
├── compile_opengl.bat        # Batch file for compilation
├── setup_opengl_environment.ps1  # Auto-setup script
└── .vscode/
    └── c_cpp_properties.json # VS Code settings
```

## Troubleshooting

### Error: "glad/gl.h: No such file or directory"
- Make sure the `glad/include` folder exists
- Check the `-Iglad/include` flag in the compilation command

### Error: "GLFW/glfw3.h: No such file or directory"
- Make sure vcpkg is installed and GLFW is installed through vcpkg
- Check the vcpkg headers path in the compilation command

### Runtime error: "glfw3.dll not found"
- Copy `glfw3.dll` from `vcpkg/installed/x64-windows/bin/` to the folder with the exe file

### Error: "Failed to create GLFW window"
- Make sure your graphics card supports OpenGL 3.3+
- Try changing the OpenGL version in the code

## Additional Resources

- [LearnOpenGL](https://learnopengl.com/) - excellent OpenGL tutorial
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [GLAD Generator](https://glad.dav1d.de/) - for generating GLAD files
