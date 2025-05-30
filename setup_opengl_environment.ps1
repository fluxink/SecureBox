# OpenGL Development Environment Setup Script
# Запускайте от имени администратора

param(
    [string]$VcpkgPath = "C:\vcpkg",
    [string]$MinGWPath = "C:\mingw64"
)

Write-Host "=== OpenGL Development Environment Setup ===" -ForegroundColor Green
Write-Host "Этот скрипт установит все необходимые компоненты для разработки OpenGL приложений"
Write-Host ""

# Проверка прав администратора
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Error "Этот скрипт требует права администратора. Запустите PowerShell от имени администратора."
    exit 1
}

# Функция для проверки наличия команды
function Test-CommandExists {
    param($Command)
    try {
        Get-Command $Command -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# Проверка Git
Write-Host "Проверка Git..." -ForegroundColor Yellow
if (-not (Test-CommandExists "git")) {
    Write-Error "Git не установлен. Установите Git с https://git-scm.com/download/win"
    exit 1
}
Write-Host "✓ Git найден" -ForegroundColor Green

# Проверка GCC/MinGW
Write-Host "Проверка GCC/MinGW..." -ForegroundColor Yellow
if (-not (Test-CommandExists "gcc")) {
    Write-Warning "GCC не найден в PATH. Пытаемся найти MinGW..."
    
    # Поиск популярных путей MinGW
    $mingwPaths = @(
        "C:\mingw64\bin",
        "C:\Program Files\mingw-w64\mingw64\bin",
        "C:\ProgramData\mingw64\mingw64\bin",
        "C:\msys64\mingw64\bin"
    )
    
    $mingwFound = $false
    foreach ($path in $mingwPaths) {
        if (Test-Path "$path\gcc.exe") {
            Write-Host "✓ MinGW найден в $path" -ForegroundColor Green
            $env:PATH = "$path;$env:PATH"
            $mingwFound = $true
            break
        }
    }
    
    if (-not $mingwFound) {
        Write-Error "MinGW не найден. Установите MinGW-w64 или добавьте его в PATH"
        Write-Host "Рекомендуется скачать с: https://www.mingw-w64.org/downloads/"
        exit 1
    }
} else {
    Write-Host "✓ GCC найден" -ForegroundColor Green
}

# Установка vcpkg
Write-Host "Установка vcpkg..." -ForegroundColor Yellow
if (Test-Path $VcpkgPath) {
    Write-Host "vcpkg уже существует в $VcpkgPath" -ForegroundColor Yellow
    $response = Read-Host "Переустановить vcpkg? (y/N)"
    if ($response -eq "y" -or $response -eq "Y") {
        Remove-Item -Recurse -Force $VcpkgPath
    } else {
        Write-Host "Пропускаем установку vcpkg" -ForegroundColor Yellow
        $skipVcpkg = $true
    }
}

if (-not $skipVcpkg) {
    try {
        Write-Host "Клонирование vcpkg..." -ForegroundColor Yellow
        git clone https://github.com/Microsoft/vcpkg.git $VcpkgPath
        
        Write-Host "Инициализация vcpkg..." -ForegroundColor Yellow
        Set-Location $VcpkgPath
        .\bootstrap-vcpkg.bat
        
        Write-Host "✓ vcpkg успешно установлен" -ForegroundColor Green
    } catch {
        Write-Error "Ошибка при установке vcpkg: $($_.Exception.Message)"
        exit 1
    }
}

# Установка GLFW через vcpkg
Write-Host "Установка GLFW3..." -ForegroundColor Yellow
try {
    Set-Location $VcpkgPath
    .\vcpkg install glfw3:x64-windows
    Write-Host "✓ GLFW3 успешно установлен" -ForegroundColor Green
} catch {
    Write-Error "Ошибка при установке GLFW3: $($_.Exception.Message)"
    exit 1
}

# Обновление переменной PATH
Write-Host "Обновление переменной PATH..." -ForegroundColor Yellow
$currentPath = [Environment]::GetEnvironmentVariable("PATH", "Machine")
$vcpkgBinPath = "$VcpkgPath"

if ($currentPath -notlike "*$vcpkgBinPath*") {
    $newPath = "$currentPath;$vcpkgBinPath"
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "Machine")
    Write-Host "✓ PATH обновлен" -ForegroundColor Green
} else {
    Write-Host "✓ vcpkg уже в PATH" -ForegroundColor Green
}

# Создание примера CMakeLists.txt
Write-Host "Создание примера CMakeLists.txt..." -ForegroundColor Yellow
$cmakeContent = @"
cmake_minimum_required(VERSION 3.20)
project(OpenGLExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск GLFW через vcpkg
find_package(glfw3 CONFIG REQUIRED)

# Добавление GLAD (предполагается, что файлы GLAD находятся в папке glad/)
add_library(glad glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# Создание исполняемого файла
add_executable(hellowindow2 hellowindow2.cpp)

# Связывание библиотек
target_link_libraries(hellowindow2 PRIVATE glfw glad)

# Копирование DLL для запуска
if(WIN32)
    add_custom_command(TARGET hellowindow2 POST_BUILD
        COMMAND `${CMAKE_COMMAND} -E copy_if_different
        "$VcpkgPath/installed/x64-windows/bin/glfw3.dll"
        `$<TARGET_FILE_DIR:hellowindow2>)
endif()
"@

# Сохраняем в текущую директорию если мы не в vcpkg
if ((Get-Location).Path -eq $VcpkgPath) {
    Set-Location $PSScriptRoot
}

$cmakeContent | Out-File -FilePath "CMakeLists_example.txt" -Encoding UTF8
Write-Host "✓ Пример CMakeLists.txt создан (CMakeLists_example.txt)" -ForegroundColor Green

# Создание batch файла для компиляции
Write-Host "Создание batch файла для компиляции..." -ForegroundColor Yellow
$batchContent = @"
@echo off
echo Компиляция OpenGL приложения...

REM Проверка наличия исходного файла
if not exist hellowindow2.cpp (
    echo Ошибка: файл hellowindow2.cpp не найден!
    pause
    exit /b 1
)

REM Проверка наличия GLAD файлов
if not exist glad\include\glad\gl.h (
    echo Ошибка: GLAD файлы не найдены! Убедитесь что папка glad\ существует.
    pause
    exit /b 1
)

REM Компиляция
echo Компилируем...
g++ hellowindow2.cpp -Iglad/include -I"$VcpkgPath/installed/x64-windows/include" glad/src/gl.c -L"$VcpkgPath/installed/x64-windows/lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

if %ERRORLEVEL% NEQ 0 (
    echo Ошибка компиляции!
    pause
    exit /b 1
)

REM Копирование DLL
echo Копируем необходимые DLL...
copy "$VcpkgPath\installed\x64-windows\bin\glfw3.dll" . >nul 2>&1

echo Компиляция завершена успешно!
echo Запуск программы...
hellowindow2.exe

pause
"@

$batchContent | Out-File -FilePath "compile_opengl.bat" -Encoding ASCII
Write-Host "✓ Batch файл создан (compile_opengl.bat)" -ForegroundColor Green

Write-Host ""
Write-Host "=== Установка завершена! ===" -ForegroundColor Green
Write-Host ""
Write-Host "Что дальше:" -ForegroundColor Cyan
Write-Host "1. Перезагрузите командную строку для обновления PATH"
Write-Host "2. Убедитесь что у вас есть файлы GLAD в папке glad/"
Write-Host "3. Для компиляции через CMake используйте:"
Write-Host "   cmake -S . -B build -G `"MinGW Makefiles`" -DCMAKE_TOOLCHAIN_FILE=$VcpkgPath/scripts/buildsystems/vcpkg.cmake"
Write-Host "   cmake --build build"
Write-Host "4. Для прямой компиляции используйте: compile_opengl.bat"
Write-Host ""
Write-Host "Документация: SETUP_GUIDE.md" -ForegroundColor Yellow
