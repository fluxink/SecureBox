@echo off
REM Script for setting up VS Code configuration

echo ==========================================
echo   VS Code Configuration Setup
echo ==========================================
echo.

REM Create .vscode folder if it doesn't exist
if not exist ".vscode" (
    mkdir .vscode
    echo Created .vscode folder
)

REM Copy IntelliSense configuration
if not exist ".vscode\c_cpp_properties.json" (
    copy c_cpp_properties_template.json .vscode\c_cpp_properties.json >nul
    echo ✓ Copied c_cpp_properties.json
) else (
    echo ! c_cpp_properties.json already exists
)

REM Copy VS Code tasks
if not exist ".vscode\tasks.json" (
    copy tasks_template.json .vscode\tasks.json >nul
    echo ✓ Copied tasks.json
) else (
    echo ! tasks.json already exists
)

echo.
echo Setup completed!
echo.
echo The following tasks are now available in VS Code:
echo - Build OpenGL with CMake (Ctrl+Shift+P → "Tasks: Run Task")
echo - Build Direct with g++
echo - Run OpenGL Application
echo - Clean Build
echo.
echo IntelliSense is also configured for correct header recognition.
echo.
pause