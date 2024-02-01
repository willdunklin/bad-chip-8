project(CHIP8)

cmake_minimum_required(VERSION 3.25)

add_executable(chip8 main.c)
add_executable(chip8asm assembler.c)

target_sources(
    chip8
    PRIVATE
        token.h
        display.h
        command.h
)

target_sources(
    chip8asm
    PRIVATE
        util.h
        token.h
        command.h
)
