@echo off
REM OpenGL + GLFW + GLAD Compilation Script
REM Убедитесь что vcpkg установлен и GLFW установлен через vcpkg

echo ==========================================
echo   OpenGL Application Compilation Script
echo ==========================================
echo.

REM Определяем путь к vcpkg (измените если нужно)
set VCPKG_PATH=D:\Pavel\vcpkg
if not exist "%VCPKG_PATH%" (
    set VCPKG_PATH=C:\vcpkg
)

if not exist "%VCPKG_PATH%" (
    echo Ошибка: vcpkg не найден!
    echo Проверьте путь к vcpkg или установите его.
    echo Ожидаемые пути: D:\Pavel\vcpkg или C:\vcpkg
    pause
    exit /b 1
)

echo Используемый путь vcpkg: %VCPKG_PATH%
echo.

REM Проверка наличия исходного файла
if not exist hellowindow2.cpp (
    echo Ошибка: файл hellowindow2.cpp не найден!
    echo Убедитесь что вы находитесь в правильной папке.
    pause
    exit /b 1
)

REM Проверка наличия GLAD файлов
if not exist glad\include\glad\gl.h (
    echo Ошибка: GLAD файлы не найдены!
    echo Убедитесь что папка glad\include\glad\ существует и содержит gl.h
    pause
    exit /b 1
)

if not exist glad\src\gl.c (
    echo Ошибка: файл glad\src\gl.c не найден!
    pause
    exit /b 1
)

REM Проверка установки GLFW в vcpkg
if not exist "%VCPKG_PATH%\installed\x64-windows\include\GLFW\glfw3.h" (
    echo Ошибка: GLFW не установлен через vcpkg!
    echo Выполните: %VCPKG_PATH%\vcpkg install glfw3:x64-windows
    pause
    exit /b 1
)

echo Все файлы найдены. Начинаем компиляцию...
echo.

REM Компиляция
echo Команда компиляции:
echo g++ hellowindow2.cpp -Iglad/include -I"%VCPKG_PATH%/installed/x64-windows/include" glad/src/gl.c -L"%VCPKG_PATH%/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe
echo.

g++ hellowindow2.cpp -Iglad/include -I"%VCPKG_PATH%/installed/x64-windows/include" glad/src/gl.c -L"%VCPKG_PATH%/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ==========================================
    echo   ОШИБКА КОМПИЛЯЦИИ!
    echo ==========================================
    echo Проверьте:
    echo 1. Установлен ли MinGW и доступен ли g++
    echo 2. Правильно ли установлен vcpkg и GLFW
    echo 3. Существуют ли все необходимые файлы
    pause
    exit /b 1
)

echo.
echo Компиляция завершена успешно!
echo.

REM Копирование DLL
echo Копируем glfw3.dll...
copy "%VCPKG_PATH%\installed\x64-windows\bin\glfw3.dll" . >nul 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo Предупреждение: Не удалось скопировать glfw3.dll
    echo Скопируйте файл вручную: %VCPKG_PATH%\installed\x64-windows\bin\glfw3.dll
    echo.
)

echo ==========================================
echo   ГОТОВО!
echo ==========================================
echo.
echo Хотите запустить программу? (Y/N)
set /p choice=Ваш выбор: 

if /i "%choice%"=="Y" (
    echo.
    echo Запускаем hellowindow2.exe...
    echo Нажмите ESC для выхода из программы.
    echo.
    hellowindow2.exe
) else (
    echo.
    echo Для запуска используйте: hellowindow2.exe
)

echo.
pause
