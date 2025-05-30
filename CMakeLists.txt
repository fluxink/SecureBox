cmake_minimum_required(VERSION 3.20)
project(CppTest)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск GLFW через vcpkg
find_package(glfw3 CONFIG REQUIRED)

# Добавление GLAD (локально)
add_library(glad glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# Создание исполняемого файла для main.cpp
add_executable(main main.cpp)

# Создание исполняемого файла для hellowindow2.cpp
add_executable(hellowindow2 hellowindow2.cpp)

# Связывание библиотек для hellowindow2
target_link_libraries(hellowindow2 PRIVATE glfw glad)

# Для отладочной информации в Debug режиме
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(main PRIVATE -g -O0)
    target_compile_options(hellowindow2 PRIVATE -g -O0)
endif()

# Для оптимизации в Release режиме
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(main PRIVATE -O3)
    target_compile_options(hellowindow2 PRIVATE -O3)
endif()
