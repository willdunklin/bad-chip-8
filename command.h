#ifndef CHIP8_COMMAND_H
#define CHIP8_COMMAND_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "instruction.h"

typedef struct {
    InstructionType type; // 16 bit opcode (enum)
    uint8_t x;            // 4 bit index into register array
    uint8_t y;            // 4 bit index into register array
    uint16_t n;           // 12 bit immediate value
} Command;

Command command_parse_opcode(uint16_t opcode) {
    Command c = {0};
    c.type = opcode; // the variables in opcodes will be extracted below

    // break 16-bit opcode into 4, 4-bit nibbles
    uint8_t nibbles[4];
    for (int i = 0; i < 4; i++) {
        uint8_t nibble = (opcode >> (12 - i*4)) & 0xF;
        nibbles[i] = nibble;
    }

    // extract opcode variables
    //   in instruction enum encoding X->0, Y->1, N->F
    switch(nibbles[0]) {
        // opcodes of the form 0x_XY_
        case 0x5:
        case 0x8:
        case 0x9:
        case 0xD:
        {
            c.type &= 0xF00F; // clear _XY_
            c.type |= 0x0010; // set   _XY_ (_01_)
            c.x = nibbles[1];
            c.y = nibbles[2];

            if (nibbles[0] == 0xD) {
                // handle degenerate case 0xDXYN
                c.type |= 0x000F; // set    ___N (___F)
                c.n = nibbles[3];
            }
            break;
        }

        // opcodes of the form 0x_NNN
        case 0x1:
        case 0x2:
        case 0xA:
        case 0xB:
        {
            c.type &= 0xF000; // clear _NNN
            c.type |= 0x0FFF; // set   _NNN (_FFF)
            c.n = (nibbles[1] << 8) | (nibbles[2] << 4) | nibbles[3];
            break;
        }

        // opcodes of the form 0x_XNN
        case 0x3:
        case 0x4:
        case 0x6:
        case 0x7:
        case 0xC:
        {
            c.type &= 0xF000; // clear _XNN
            c.type |= 0x00FF; // set   _XNN (_0FF)
            c.x = nibbles[1];
            c.n = (nibbles[2] << 4) | nibbles[3];
            break;
        }

        // opcodes of the form 0x_X__
        case 0xE:
        case 0xF:
        {
            c.type &= 0xF0FF; // clear _X__
            c.type |= 0x0000; // set   _X__ (_0__)
            c.x = nibbles[1];
            c.n = (nibbles[2] << 4) | nibbles[3];
            break;
        }

        // opcodes of the form 0x0___
        case 0x0: break; // no variables need to be set
    }

    return c;
}

void command_print(Command c) {
    printf("command:\n");
    printf("  type: 0x%X\n", c.type);
    printf("  x:    0x%X\n", c.x);
    printf("  y:    0x%X\n", c.y);
    printf("  n:    0x%X\n", c.n);
    printf("\n");
}

void command_opcode_debug(uint16_t opcode) {
    printf("opcode:\n  0x%X\n", opcode);
    command_print(command_parse_opcode(opcode));
}

#endif // CHIP8_COMMAND_H
