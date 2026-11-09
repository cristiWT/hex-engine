cmake_minimum_required(VERSION 3.20)
project(hex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CSS_STANDARD_REQUIRED ON)
set(CMAKE_CSS_EXTENSIONS OFF)

add_library(hexcore src/board.cpp)
target_include_directories(hexcore PUBLIC src)
targe_compile_options(hexcore PRIVATE -Wall -Wextra -Wpedantic)

add_executable(hex src/main.cpp)
target_link_libraries(hex PRIVATE hexcore)
