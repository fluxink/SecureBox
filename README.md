# SecureBox - Automatic Puzzle Solver with 3D Visualization

SecureBox is a demonstration program that automatically solves a mathematical puzzle using linear algebra. The program generates a random locked grid and then calculates and visualizes the optimal solution step by step.

## 🧮 What is SecureBox?

SecureBox represents a 2D grid puzzle where each cell has three states:
- **0** - Fully unlocked (green)
- **1** - Partially locked (yellow) 
- **2** - Fully locked (red)

**Goal:** Automatically find a sequence of toggle operations to unlock all cells (make them all 0).

**Toggle Rules:** When position (x,y) is toggled:
- All cells in row y are incremented by 1 (mod 3)
- All cells in column x are incremented by 1 (mod 3)

**Solution Method:** The program uses linear algebra to solve the system of equations and find the optimal solution automatically.

## 🚀 Quick Start

### Run the Solver
```cmd
# Build and run with dual visualization (Console + 3D OpenGL)
compile_opengl.bat
securebox.exe 4 3

# Or console-only mode
securebox.exe 4 3 --console
```

### Setup Development Environment
```powershell
# Run as Administrator
.\setup_opengl_environment.ps1
setup_vscode.bat
```

## 📁 Project Structure

```
├── main.cpp                    # SecureBox solver with 3D visualization
├── hellowindow2.cpp            # Simple OpenGL example
├── CMakeLists.txt              # Build configuration
├── shaders/example.glsl        # GLSL shaders for 3D rendering
├── glad/                       # OpenGL loader
```

## 🎯 Visualization Modes

### Dual Mode (Default)
- **Console**: Step-by-step text output showing the solving process
- **3D OpenGL**: Real-time animated visualization with shader effects
- **Interaction**: Press SPACE in the OpenGL window to advance to next step

### Console Mode (`--console`)
- Text-only interface with colored grid display
- **Interaction**: Press ENTER to advance to next step
- Shows the mathematical solving process step by step

## 🛠 Build Options

```cmd
# Quick build (recommended)
compile_opengl.bat

# Using CMake
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build

# Direct compilation
g++ main.cpp -Iglad/include -I"C:\vcpkg\installed\x64-windows\include" glad/src/gl.c -L"C:\vcpkg\installed\x64-windows\lib" 
-lglfw3dll -lgdi32 -lopengl32 -o securebox.exe 
```

## 🎨 Demonstration

### Console Visualization
<!-- Add your console GIF here -->
*Console mode showing the automatic solving process with colored text output and step-by-step mathematical solution*

### 3D OpenGL Visualization  
<!-- Add your OpenGL GIF here -->
*3D animated visualization showing the solving algorithm in action with shader effects and smooth transitions*

## 🧮 How It Works

1. **Grid Generation**: Creates a random locked grid using pseudo-random toggle operations
2. **Problem Analysis**: Converts the puzzle into a system of linear equations (mod 3)
3. **Solution Calculation**: Uses linear algebra to find the optimal sequence of toggles
4. **Visualization**: Shows the solution being applied step by step in both console and 3D modes
5. **Verification**: Confirms that all cells are successfully unlocked

The program demonstrates how mathematical puzzles can be solved algorithmically and visualized beautifully.

## 🔧 Requirements

- **Windows 10/11**
- **MinGW-w64** (GCC compiler)
- **OpenGL 3.3+** support
- **GLFW3** and **GLAD** (auto-installed by setup script)

## 📖 More Information

- [SETUP_GUIDE.md](SETUP_GUIDE.md) - Detailed installation and troubleshooting
- [CHECKLIST.md](CHECKLIST.md) - Development checklist
