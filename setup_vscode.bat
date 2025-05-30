@echo off
REM Скрипт для настройки VS Code конфигурации

echo ==========================================
echo   VS Code Configuration Setup
echo ==========================================
echo.

REM Создаем папку .vscode если её нет
if not exist ".vscode" (
    mkdir .vscode
    echo Создана папка .vscode
)

REM Копируем конфигурацию IntelliSense
if not exist ".vscode\c_cpp_properties.json" (
    copy c_cpp_properties_template.json .vscode\c_cpp_properties.json >nul
    echo ✓ Скопирован c_cpp_properties.json
) else (
    echo ! c_cpp_properties.json уже существует
)

REM Копируем задачи VS Code
if not exist ".vscode\tasks.json" (
    copy tasks_template.json .vscode\tasks.json >nul
    echo ✓ Скопирован tasks.json
) else (
    echo ! tasks.json уже существует
)

echo.
echo Настройка завершена!
echo.
echo Теперь в VS Code доступны следующие задачи:
echo - Build OpenGL with CMake (Ctrl+Shift+P → "Tasks: Run Task")
echo - Build Direct with g++
echo - Run OpenGL Application
echo - Clean Build
echo.
echo Также настроен IntelliSense для корректного распознавания заголовков.
echo.
pause
