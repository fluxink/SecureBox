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
g++ main.cpp -Iglad/include -I"C:\vcpkg/installed/x64-windows/include" glad/src/gl.c -L"C:\vcpkg/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o securebox.exe

if %ERRORLEVEL% NEQ 0 (
    echo Compilation error!
    pause
    exit /b 1
)

REM Copy DLL
echo Copying required DLLs...
copy "C:\vcpkg\installed\x64-windows\bin\glfw3.dll" . >nul 2>&1

echo Compilation completed successfully!
echo Running program...
securebox.exe

pause
