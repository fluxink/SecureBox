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


**Solution Method:** 
1. **Model as System of Equations**: Each cell (i,j) represents an equation in Galois Field GF(3), where all arithmetic is done modulo 3

2. **Effect Matrix**: Creates a lookup table showing how each possible toggle(x,y) affects every cell in the grid:
   ```
   Example for 2×2 grid:
   Grid:        Possible toggles:
   [A] [B]      toggle(0,0)  toggle(1,0)  toggle(0,1)  toggle(1,1)
   [C] [D]      
   
   Effect Matrix:
              toggle(0,0)  toggle(1,0)  toggle(0,1)  toggle(1,1)
   cell A:         2           1           1           0
   cell B:         1           2           0           1  
   cell C:         1           0           2           1
   cell D:         0           1           1           2
   ```

3. **Toggle Rules**: toggle(x,y) increments by +1 (mod 3):
   - All cells in column x (vertical effect)
   - All cells in row y (horizontal effect)  
   - Cell (x,y) gets additional +2, totaling +1 mod 3 (compensated double increment)

4. **Target Vector**: Calculates how much to add to each cell to make it 0:
   ```
   Current state:    [1] [2] [0]
                     [2] [1] [1]
   
   Target vector:    [2] [1] [0]  ← what to add to get [0] [0] [0]
                     [1] [2] [2]  ← what to add to get [0] [0] [0]
   ```
   - If cell = 0, add 0 → (0 + 0) % 3 = 0 ✓
   - If cell = 1, add 2 → (1 + 2) % 3 = 0 ✓
   - If cell = 2, add 1 → (2 + 1) % 3 = 0 ✓

5. **Solve**: Uses Gaussian elimination in GF(3) to find how many times to toggle each position

6. **Execute**: Applies the calculated toggles step-by-step until all cells become 0 (unlocked)


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
├── main.cpp                    # SecureBox solver with 3D 
├── hellowindow2.cpp            # Simple OpenGL example
├── CMakeLists.txt              # Build configuration
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

### 3D OpenGL Visualization  
![3D Demo](media/OpenGL.gif)


## 🔧 Requirements

- **Windows 10/11**
- **MinGW-w64** (GCC compiler)
- **OpenGL 3.3+** support
- **GLFW3** and **GLAD** (auto-installed by setup script)

## 📖 More Information

- [SETUP_GUIDE.md](SETUP_GUIDE.md) - Detailed installation and troubleshooting
- [CHECKLIST.md](CHECKLIST.md) - Development checklist
