#ifndef CHIP8_COMMAND_H
#define CHIP8_COMMAND_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "instruction.h"

typedef struct {
    instruction type; // 16 bit opcode (enum)
    uint8_t x;        // 4 bit index into register array
    uint8_t y;        // 4 bit index into register array
    uint16_t n;       // 12 bit immediate value
} command;

void command_print(command c);
void command_opcode_debug(uint16_t opcode);

command command_parse_opcode(uint16_t opcode);

#endif // CHIP8_COMMAND_H
