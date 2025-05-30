# C++ Test Project

Простой C++ проект для тестирования и изучения.

## Требования

- Windows 11
- MinGW (GCC 13.2.0)
- CMake (4.0.2)
- VS Code (рекомендуется)

## Сборка проекта

### Вариант 1: С использованием CMake

```bash
# Создание папки для сборки
mkdir build
cd build

# Генерация файлов сборки
cmake .. -G "MinGW Makefiles"

# Сборка проекта
mingw32-make

# Запуск программы
./main.exe
```

### Вариант 2: Прямая компиляция

```bash
# Компиляция
g++ -std=c++17 -O2 main.cpp -o main.exe

# Запуск
./main.exe
```

## Структура проекта

- `main.cpp` - основной файл с кодом
- `CMakeLists.txt` - файл конфигурации CMake
- `build/` - папка для скомпилированных файлов
- `.vscode/` - настройки VS Code

## VS Code

Рекомендуемые расширения:
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- Code Runner
