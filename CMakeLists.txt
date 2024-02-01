project(CHIP8)

cmake_minimum_required(VERSION 3.25)

# add_library(command command.c command.h instruction.h)
add_executable(chip8 main.c)
add_executable(chip8asm assembler.c)

target_sources(
    chip8
    PRIVATE
        instruction.h
        display.h
        command.h
)

target_sources(
    chip8asm
    PRIVATE
        util.h
        token.h
        instruction.h
        command.h
)
