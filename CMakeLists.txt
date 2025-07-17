cmake_minimum_required(VERSION 3.31)
project(chip8emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(chip8emulator main.cpp
        src/Chip.cpp
        src/Chip.h)

