# ✅ Checklist for New Users

## 📥 Step 1: Getting the Project
- [ ] Download or clone the project
- [ ] Make sure all files are in place (see DEPLOYMENT_SUMMARY.md)

## 🔧 Step 2: Automatic Environment Setup
- [ ] Open **PowerShell as Administrator**
- [ ] Navigate to the project folder: `cd path\to\project`
- [ ] Run: `.\setup_opengl_environment.ps1`
- [ ] Wait for installation to complete
- [ ] **Restart command prompt** to update PATH

## 🎨 Step 3: VS Code Setup (optional)
- [ ] Open command prompt in the project folder
- [ ] Run: `setup_vscode.bat`
- [ ] Open project in VS Code
- [ ] Make sure IntelliSense works (no red underlines in `#include <GLFW/glfw3.h>`)

## 🚀 Step 4: Compilation and Running
- [ ] **Easy way**: Double-click on `compile_opengl.bat`
- [ ] **Or** in command line: `compile_opengl.bat`
- [ ] Make sure the program compiled without errors
- [ ] Run `hellowindow2.exe` - an OpenGL window should open
- [ ] Press ESC to close the window

## 🔍 Step 5: Functionality Check
- [ ] OpenGL window opens with greenish-gray background
- [ ] Console outputs: "Starting GLFW context, OpenGL 3.3" and "Loaded OpenGL 3.3"
- [ ] Window responds to ESC key press (closes)
- [ ] No IntelliSense errors in VS Code for OpenGL code

## 🆘 If Something Doesn't Work

### Error: "vcpkg not found"
- [ ] Check that PowerShell is running as Administrator
- [ ] Make sure you have internet connection to download vcpkg
- [ ] Try installing vcpkg manually (see SETUP_GUIDE.md)

### Error: "GCC not found"
- [ ] Install MinGW-w64
- [ ] Make sure MinGW is added to PATH
- [ ] Restart command prompt

### Error: "GLFW/glfw3.h not found"
- [ ] Make sure vcpkg installed GLFW: `C:\vcpkg\vcpkg list`
- [ ] Check the vcpkg path in `compile_opengl.bat`
- [ ] Try reinstalling GLFW: `C:\vcpkg\vcpkg install glfw3:x64-windows`

### IntelliSense doesn't work in VS Code
- [ ] Make sure `setup_vscode.bat` was run
- [ ] Check that `.vscode\c_cpp_properties.json` file was created
- [ ] Reload VS Code: Ctrl+Shift+P → "Developer: Reload Window"

### Program doesn't start: "glfw3.dll not found"
- [ ] Make sure `glfw3.dll` is in the same folder as `hellowindow2.exe`
- [ ] Copy DLL manually: `copy "C:\vcpkg\installed\x64-windows\bin\glfw3.dll" .`

## 📖 Additional Help
- Read **SETUP_GUIDE.md** for detailed instructions
- Check the "Troubleshooting" section in documentation
- Make sure your graphics card supports OpenGL 3.3+

## 🎉 Success!
If all steps are completed and the OpenGL window opens - congratulations! 
You're ready to learn OpenGL programming.

**Next steps:**
- Study the code in `hellowindow2.cpp`
- Try changing the background color (`glClearColor` line)
- Visit https://learnopengl.com/ for further learning
