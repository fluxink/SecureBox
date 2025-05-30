cmake_minimum_required(VERSION 3.20)
project(CppTest)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создание исполняемого файла
add_executable(main main.cpp)

# Если вы хотите добавить дополнительные исходные файлы:
# add_executable(main main.cpp other_file.cpp)

# Для отладочной информации в Debug режиме
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(main PRIVATE -g -O0)
endif()

# Для оптимизации в Release режиме
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(main PRIVATE -O3)
endif()
