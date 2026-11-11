cmake_minimum_required(VERSION 3.20)
project(hex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sanitizer
option(HEX_SANITIZE "Build with sanitizers" OFF)
if(HEX_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(hexcore src/board.cpp src/union_find.cpp)
target_include_directories(hexcore PUBLIC src)
target_compile_options(hexcore PRIVATE -Wall -Wextra -Wpedantic)

add_executable(hex src/main.cpp)
target_link_libraries(hex PRIVATE hexcore)
target_compile_options(hex PRIVATE -Wall -Wextra -Wpedantic)

# Tests
include(FetchContent)
FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1)
FetchContent_MakeAvailable(Catch2)

enable_testing()
add_executable(tests tests/test_board.cpp tests/test_union_find.cpp)
target_link_libraries(tests PRIVATE hexcore Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(tests)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)