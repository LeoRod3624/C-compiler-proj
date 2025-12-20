cmake_minimum_required(VERSION 3.16)

project(leocc LANGUAGES CXX)

# Use C++17 (or bump if you like)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# If you want to force clang++ instead of g++, you can uncomment these:
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

add_executable(leocc
    main.cpp
    tokenizer.cpp
    ast.cpp
    codegen.cpp
    leocc.hpp
    codegen_llvm.cpp
)

# Use the same LLVM include dirs as your Makefile
target_include_directories(leocc PRIVATE
    /usr/include/llvm-21
    /usr/include/llvm-c-21
)

# Link against the same LLVM lib
target_link_libraries(leocc PRIVATE
    LLVM-21
)
