cmake_minimum_required(VERSION 3.16)

project(HomeoPro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
qt_standard_project_setup()

add_subdirectory(src)
add_subdirectory(tests)


