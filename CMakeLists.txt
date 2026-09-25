cmake_minimum_required(VERSION 3.20)

project(asmc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(asmc
    src/main.cpp
    src/cpu/InstructionSet.cpp
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
    src/assembler/Assembler.cpp
    src/compiler/Compiler.cpp
)

target_include_directories(asmc PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(asmc PRIVATE /W4)
else()
    target_compile_options(asmc PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()