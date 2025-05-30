# C++ Test Project with OpenGL Support

This project demonstrates setting up a development environment for C++ with OpenGL, GLFW, and GLAD support.

## 🚀 Quick Start

### Automatic Installation (Recommended)

1. **Run PowerShell as Administrator**
2. **Execute automatic setup:**
   ```powershell
   .\setup_opengl_environment.ps1
   ```

### VS Code Setup
```cmd
setup_vscode.bat
```

### Compilation and Execution
```cmd
REM Method 1: Batch file (simplest)
compile_opengl.bat

REM Method 2: CMake
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build
.\build\hellowindow2.exe

REM Method 3: Direct compilation
g++ hellowindow2.cpp -Iglad/include -I"C:\vcpkg\installed\x64-windows\include" glad/src/gl.c -L"C:\vcpkg\installed\x64-windows\lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe
```

## 📁 Project structure

```
├── main.cpp                          # Простой пример C++
├── hellowindow2.cpp                   # OpenGL пример с GLFW и GLAD
├── CMakeLists.txt                     # CMake конфигурация
│
├── 📋 Documentation and Instructions
├── README.md                          # This file
├── SETUP_GUIDE.md                     # Detailed installation guide
│
├── 🔧 Automation Scripts
├── setup_opengl_environment.ps1      # Automatic environment setup
├── compile_opengl.bat                 # Simple OpenGL compilation
├── setup_vscode.bat                   # VS Code configuration setup
│
├── 📋 Configuration Templates
├── tasks_template.json               # VS Code tasks template
├── c_cpp_properties_template.json    # VS Code IntelliSense template
│
├── 📚 Libraries
├── glad/                             # GLAD OpenGL loader
│   ├── include/
│   └── src/
│
└── 🔨 Build
    ├── build/                        # CMake build files
    ├── *.exe                         # Compiled programs
    └── *.dll                         # Required libraries
```

## 🛠 Available Scripts

| Script | Description |
|--------|----------|
| `setup_opengl_environment.ps1` | Complete automatic environment setup |
| `compile_opengl.bat` | OpenGL project compilation (simplest method) |
| `setup_vscode.bat` | VS Code configuration setup |

## 📖 Detailed Documentation

See [SETUP_GUIDE.md](SETUP_GUIDE.md) for:
- Detailed manual installation instructions
- Troubleshooting
- Project structure explanation
- Additional resources

## 🎯 Примеры

### Basic C++ Example
```cpp
// main.cpp - simple Hello World
#include <iostream>
int main() {
    std::cout << "Hello, C++!" << std::endl;
    return 0;
}
```

### OpenGL Example
```cpp
// hellowindow2.cpp - creating OpenGL window with GLFW and GLAD
// Demonstrates OpenGL initialization and basic render loop
```

## 🔧 Requirements

- **Windows 10/11**
- **MinGW-w64** (GCC compiler)
- **Git**
- **PowerShell** (for automatic setup)
- **Visual Studio Code** (optional, for development)

## 🏃‍♂️ For the Impatient

```cmd
REM 1. Download the project
git clone <your-repo-url>
cd cpp_test

REM 2. Run automatic setup (PowerShell as Administrator)
.\setup_opengl_environment.ps1

REM 3. Setup VS Code (optional)
setup_vscode.bat

REM 4. Compile and run
compile_opengl.bat
```

## 🆘 Help

If something doesn't work:
1. Check [SETUP_GUIDE.md](SETUP_GUIDE.md) "Troubleshooting" section
2. Make sure all requirements are installed
3. Check paths in scripts (especially vcpkg path)

---

**Tip:** Use `compile_opengl.bat` for quick compilation without CMake setup!
