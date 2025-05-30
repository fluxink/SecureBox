@echo off
REM OpenGL + GLFW + GLAD Compilation Script
REM Make sure vcpkg is installed and GLFW is installed through vcpkg

echo ==========================================
echo   OpenGL Application Compilation Script
echo ==========================================
echo.

REM Define vcpkg path (change if needed)
set VCPKG_PATH=D:\Pavel\vcpkg
if not exist "%VCPKG_PATH%" (
    set VCPKG_PATH=C:\vcpkg
)

if not exist "%VCPKG_PATH%" (
    echo Error: vcpkg not found!
    echo Check vcpkg path or install it.
    echo Expected paths: D:\Pavel\vcpkg or C:\vcpkg
    pause
    exit /b 1
)

echo Using vcpkg path: %VCPKG_PATH%
echo.

REM Check for source file
if not exist hellowindow2.cpp (
    echo Error: hellowindow2.cpp file not found!
    echo Make sure you are in the correct folder.
    pause
    exit /b 1
)

REM Check for GLAD files
if not exist glad\include\glad\gl.h (
    echo Error: GLAD files not found!
    echo Make sure glad\include\glad\ folder exists and contains gl.h
    pause
    exit /b 1
)

if not exist glad\src\gl.c (
    echo Error: glad\src\gl.c file not found!
    pause
    exit /b 1
)

REM Check GLFW installation in vcpkg
if not exist "%VCPKG_PATH%\installed\x64-windows\include\GLFW\glfw3.h" (
    echo Error: GLFW not installed through vcpkg!
    echo Execute: %VCPKG_PATH%\vcpkg install glfw3:x64-windows
    pause
    exit /b 1
)

echo All files found. Starting compilation...
echo.

REM Compilation
echo Compilation command:
echo g++ hellowindow2.cpp -Iglad/include -I"%VCPKG_PATH%/installed/x64-windows/include" glad/src/gl.c -L"%VCPKG_PATH%/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe
echo.

g++ hellowindow2.cpp -Iglad/include -I"%VCPKG_PATH%/installed/x64-windows/include" glad/src/gl.c -L"%VCPKG_PATH%/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ==========================================
    echo   COMPILATION ERROR!
    echo ==========================================
    echo Check:
    echo 1. Is MinGW installed and g++ available
    echo 2. Is vcpkg and GLFW properly installed
    echo 3. Do all required files exist
    pause
    exit /b 1
)

echo.
echo Compilation completed successfully!
echo.

REM Copy DLL
echo Copying glfw3.dll...
copy "%VCPKG_PATH%\installed\x64-windows\bin\glfw3.dll" . >nul 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo Warning: Could not copy glfw3.dll
    echo Copy file manually: %VCPKG_PATH%\installed\x64-windows\bin\glfw3.dll
    echo.
)

echo ==========================================
echo   DONE!
echo ==========================================
echo.
echo Do you want to run the program? (Y/N)
set /p choice=Your choice: 

if /i "%choice%"=="Y" (
    echo.
    echo Running hellowindow2.exe...
    echo Press ESC to exit the program.
    echo.
    hellowindow2.exe
) else (
    echo.
    echo To run use: hellowindow2.exe
)

echo.
pause