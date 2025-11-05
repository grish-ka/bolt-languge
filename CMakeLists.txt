# This is the build script for *just* the compiler
cmake_minimum_required(VERSION 3.14)
project(bolt-compiler LANGUAGES CXX)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Find Source Files ---
# Define our executable by explicitly listing all source files.
# We've added codegen.cpp to this list.
add_executable(bolt-compiler
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/codegen.cpp
)

# --- Find Dependencies ---
# Add the 'src' directory as an include path
# so we can use #include "lexer.hpp", "parser.hpp", etc.
target_include_directories(bolt-compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- Linking ---
# <filesystem> support is handled natively by modern compilers.