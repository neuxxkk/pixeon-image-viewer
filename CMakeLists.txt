# CMakeLists.txt (raiz)
cmake_minimum_required(VERSION 3.16)
project(PixeonImageViewer VERSION 1.0 LANGUAGES CXX)

# Diz ao CMake para gerar um arquivo JSON com todos os caminhos de include
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ 17 padão indústria (Stack Overflow)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# default settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# localiza Qt 6 
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

add_subdirectory(src)
add_subdirectory(tests)
