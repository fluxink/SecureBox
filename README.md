# SecureBox - 3D Puzzle Solver

Automatic solver for mathematical grid puzzles with real-time 3D visualization.

![Demo](media/OpenGL.gif)

## Quick Start
```cmd
compile_opengl.bat
securebox.exe 4 3
```

## Features
- Solves grid puzzles using linear algebra (GF3)
- Real-time 3D OpenGL visualization
- Console mode
- Interactive step-by-step solving

## Usage
```cmd
securebox.exe <width> <height> [--console]
```

## Requirements
- Windows 10/11, MinGW-w64, OpenGL 3.3+

## Documentation
- [Algorithm Details](docs/ALGORITHM.md)
- [Development](docs/CHECKLIST.md)