project(CHIP8)

cmake_minimum_required(VERSION 3.25)

# add_library(command command.c command.h instruction.h)
add_executable(chip8)

target_sources(
    chip8
    PRIVATE
        command.h
        command.c
        instruction.h
        main.c
)
