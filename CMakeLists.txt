cmake_minimum_required(VERSION 3.20)
project(CppTest)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка vcpkg toolchain (если используется)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

# Поиск GLFW через vcpkg
find_package(glfw3 CONFIG QUIET)

if(NOT glfw3_FOUND)
  # Если vcpkg не найден, попробуем прямые пути
  set(GLFW_INCLUDE_DIRS "C:/vcpkg/installed/x64-windows/include")
  set(GLFW_LIBRARIES "C:/vcpkg/installed/x64-windows/lib/glfw3dll.lib")
  
  # Создаем импортированную цель для совместимости
  add_library(glfw3_target INTERFACE)
  target_include_directories(glfw3_target INTERFACE ${GLFW_INCLUDE_DIRS})
  target_link_libraries(glfw3_target INTERFACE ${GLFW_LIBRARIES})
  add_library(glfw::glfw ALIAS glfw3_target)
  
  message(STATUS "Using manual GLFW paths")
else()
  message(STATUS "Found GLFW via vcpkg")
endif()

# Добавление GLAD (локально)
add_library(glad glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)

# Создание исполняемого файла для main.cpp
add_executable(main main.cpp)

# Создание исполняемого файла для hellowindow2.cpp
add_executable(hellowindow2 hellowindow2.cpp)

# Связывание библиотек для main (если нужны OpenGL библиотеки)
target_link_libraries(main PRIVATE glfw::glfw glad opengl32 gdi32)

# Связывание библиотек для hellowindow2
target_link_libraries(hellowindow2 PRIVATE glfw::glfw glad opengl32 gdi32)

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
