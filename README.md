# C++ Test Project with OpenGL Support

Этот проект демонстрирует настройку среды разработки для C++ с поддержкой OpenGL, GLFW и GLAD.

## 🚀 Быстрый старт

### Автоматическая установка (рекомендуется)

1. **Запустите PowerShell от имени администратора**
2. **Выполните автоустановку:**
   ```powershell
   .\setup_opengl_environment.ps1
   ```

### Настройка VS Code
```cmd
setup_vscode.bat
```

### Компиляция и запуск
```cmd
REM Метод 1: Batch файл (самый простой)
compile_opengl.bat

REM Метод 2: CMake
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build
.\build\hellowindow2.exe

REM Метод 3: Прямая компиляция
g++ hellowindow2.cpp -Iglad/include -I"C:\vcpkg\installed\x64-windows\include" glad/src/gl.c -L"C:\vcpkg\installed\x64-windows\lib" -lglfw3dll -lgdi32 -lopengl32 -o hellowindow2.exe
```

## 📁 Структура проекта

```
├── main.cpp                          # Простой пример C++
├── hellowindow2.cpp                   # OpenGL пример с GLFW и GLAD
├── CMakeLists.txt                     # CMake конфигурация
│
├── 📋 Документация и инструкции
├── README.md                          # Этот файл
├── SETUP_GUIDE.md                     # Подробное руководство по установке
│
├── 🔧 Скрипты автоматизации
├── setup_opengl_environment.ps1      # Автоустановка всего окружения
├── compile_opengl.bat                 # Простая компиляция OpenGL
├── setup_vscode.bat                   # Настройка VS Code конфигурации
│
├── 📋 Шаблоны конфигурации
├── tasks_template.json               # Шаблон задач для VS Code
├── c_cpp_properties_template.json    # Шаблон IntelliSense для VS Code
│
├── 📚 Библиотеки
├── glad/                             # GLAD OpenGL loader
│   ├── include/
│   └── src/
│
└── 🔨 Сборка
    ├── build/                        # CMake build файлы
    ├── *.exe                         # Скомпилированные программы
    └── *.dll                         # Необходимые библиотеки
```

## 🛠 Доступные скрипты

| Скрипт | Описание |
|--------|----------|
| `setup_opengl_environment.ps1` | Полная автоматическая установка окружения |
| `compile_opengl.bat` | Компиляция OpenGL проекта (самый простой способ) |
| `setup_vscode.bat` | Настройка конфигурации VS Code |

## 📖 Подробная документация

Смотрите [SETUP_GUIDE.md](SETUP_GUIDE.md) для:
- Подробных инструкций по ручной установке
- Решения проблем
- Объяснения структуры проекта
- Дополнительных ресурсов

## 🎯 Примеры

### Основной C++ пример
```cpp
// main.cpp - простой Hello World
#include <iostream>
int main() {
    std::cout << "Hello, C++!" << std::endl;
    return 0;
}
```

### OpenGL пример
```cpp
// hellowindow2.cpp - создание окна OpenGL с GLFW и GLAD
// Демонстрирует инициализацию OpenGL и основной цикл рендеринга
```

## 🔧 Требования

- **Windows 10/11**
- **MinGW-w64** (GCC компилятор)
- **Git**
- **PowerShell** (для автоустановки)
- **Visual Studio Code** (опционально, для разработки)

## 🏃‍♂️ Для нетерпеливых

```cmd
REM 1. Скачайте проект
git clone <your-repo-url>
cd cpp_test

REM 2. Запустите автоустановку (PowerShell от имени администратора)
.\setup_opengl_environment.ps1

REM 3. Настройте VS Code (опционально)
setup_vscode.bat

REM 4. Скомпилируйте и запустите
compile_opengl.bat
```

## 🆘 Помощь

Если что-то не работает:
1. Проверьте [SETUP_GUIDE.md](SETUP_GUIDE.md) раздел "Устранение проблем"
2. Убедитесь что все требования установлены
3. Проверьте пути в скриптах (особенно путь к vcpkg)

---

**Совет:** Используйте `compile_opengl.bat` для быстрой компиляции без настройки CMake!
