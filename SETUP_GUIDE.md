# OpenGL + GLFW + GLAD Setup Guide

Это руководство поможет настроить среду разработки для OpenGL с GLFW и GLAD на Windows.

## Требования

- Windows 10/11
- MinGW-w64 (GCC)
- Git
- CMake (опционально)
- Visual Studio Code (опционально)

## Быстрая автоматическая установка

Запустите PowerShell от имени администратора и выполните:

```powershell
.\setup_opengl_environment.ps1
```

## Ручная установка

### 1. Установка vcpkg

```powershell
# Клонируем vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Инициализируем vcpkg
.\bootstrap-vcpkg.bat

# Устанавливаем GLFW
.\vcpkg install glfw3:x64-windows
```

### 2. Установка MinGW-w64 (если не установлен)

Скачайте и установите MinGW-w64 с официального сайта или через пакетный менеджер.

### 3. Настройка переменных окружения

Добавьте в PATH:
- Путь к MinGW-w64 bin (например: `C:\mingw64\bin`)
- Путь к vcpkg (например: `C:\vcpkg`)

## Компиляция проекта

### Метод 1: CMake (рекомендуется)

```powershell
# Настройка проекта
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

# Сборка
cmake --build build

# Запуск
.\build\hellowindow2.exe
```

### Метод 2: Прямая компиляция

```powershell
# Компиляция
g++ hellowindow2.cpp -Iglad/include -I"C:\vcpkg\installed\x64-windows\include" glad/src/gl.c -L"C:\vcpkg\installed\x64-windows\lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe

# Копирование DLL
copy "C:\vcpkg\installed\x64-windows\bin\glfw3.dll" .

# Запуск
.\hellowindow2.exe
```

### Метод 3: Использование bat файла

```cmd
compile_opengl.bat
```

## Настройка VS Code

Для корректной работы IntelliSense в VS Code, убедитесь что в `.vscode/c_cpp_properties.json` указаны правильные пути:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/glad/include",
                "C:/vcpkg/installed/x64-windows/include"
            ],
            "compilerPath": "C:/mingw64/bin/gcc.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

## Структура проекта

```
project/
├── hellowindow2.cpp          # Основной исходный файл
├── CMakeLists.txt            # CMake конфигурация
├── glad/                     # GLAD библиотека
│   ├── include/
│   │   ├── glad/gl.h
│   │   └── KHR/khrplatform.h
│   └── src/gl.c
├── compile_opengl.bat        # Batch файл для компиляции
├── setup_opengl_environment.ps1  # Скрипт автоустановки
└── .vscode/
    └── c_cpp_properties.json # Настройки VS Code
```

## Устранение проблем

### Ошибка: "glad/gl.h: No such file or directory"
- Убедитесь, что папка `glad/include` существует
- Проверьте флаг `-Iglad/include` в команде компиляции

### Ошибка: "GLFW/glfw3.h: No such file or directory"
- Убедитесь, что vcpkg установлен и GLFW установлен через vcpkg
- Проверьте путь к заголовкам vcpkg в команде компиляции

### Ошибка запуска: "glfw3.dll not found"
- Скопируйте `glfw3.dll` из `vcpkg/installed/x64-windows/bin/` в папку с exe файлом

### Ошибка: "Failed to create GLFW window"
- Убедитесь, что ваша видеокарта поддерживает OpenGL 3.3+
- Попробуйте изменить версию OpenGL в коде

## Дополнительные ресурсы

- [LearnOpenGL](https://learnopengl.com/) - отличный учебник по OpenGL
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [GLAD Generator](https://glad.dav1d.de/) - для генерации GLAD файлов
