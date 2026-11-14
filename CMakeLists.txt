cmake_minimum_required(VERSION 3.20)
project(hex LANGUAGES CXX)

# --- settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# --- sanitizer ---
option(HEX_SANITIZE "Build with sanitizers" OFF)
if(HEX_SANITIZE)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# --- warnings ---
add_library(hex_warnings INTERFACE)
target_compile_options(hex_warnings INTERFACE -Wall -Wextra -Wpedantic)

# --- libraries ---
add_library(hexcore
        src/board.cpp
        src/union_find.cpp
        src/arena.cpp
        src/players/random.cpp
)
target_include_directories(hexcore PUBLIC src)
target_link_libraries(hexcore PRIVATE hex_warnings)

# --- executables ---
add_executable(hex src/main.cpp)
target_link_libraries(hex PRIVATE hexcore hex_warnings)
add_executable(arena src/main_arena.cpp)
target_link_libraries(arena PRIVATE hexcore hex_warnings)

# Tests
include(FetchContent)
FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1)
FetchContent_MakeAvailable(Catch2)

enable_testing()
add_executable(tests
        tests/test_board.cpp
        tests/test_union_find.cpp
        tests/test_wilson.cpp
        tests/test_arena.cpp
        tests/test_players.cpp
)
target_link_libraries(tests PRIVATE hexcore hex_warnings Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(tests)