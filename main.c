#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "instruction.h"
#include "command.h"

#define INSTRUCTION_START 0x200

#define V(X)  registers[X]  // V0-VF (must use numeric index 0-15, 0x0-0xf)


uint8_t memory[4096];
uint8_t registers[16]; // V0-VF
uint16_t I;            // index register (used for memory addresses)
uint16_t pc;           // program counter
uint8_t sp;            // stack pointer
uint8_t skip_next = 0; // additional flad used in conditionals

uint8_t delay_timer; // TODO: decrement at 60hz
uint8_t sound_timer; // TODO: decrement at 60hz


void clear_display() {
    printf("clear_display\n");
}

void run_command(command c) {
    // skip_next = 0;

    switch(c.type) {
        case I_00E0:  // clear
        {
            clear_display();
            break;
        }
        case I_00EE:  // return
        {
            pc = memory[sp];
            sp -= 1;
            break;
        }
        case I_1NNN:  // jump NNN
        {
            pc = c.n;
            break;
        }
        case I_2NNN:  // NNN
        {
            sp += 1;
            memory[sp] = pc;
            pc = c.n;
            break;
        }

        case I_3XNN:  // if VX == NN then
        {
            if(V(c.x) == (c.n & 0xFF)) skip_next = 1;
            break;
        }
        case I_4XNN:  // if VX != NN then
        {
            if(V(c.x) != (c.n & 0xFF)) skip_next = 1;
            break;
        }
        case I_5XY0:  // if VX != VY then
        {
            if(V(c.x) != V(c.y))       skip_next = 1;
            break;
        }

        case I_6XNN:  // VX = NN
        {
            V(c.x) = c.n & 0xFF;
            break;
        }
        case I_7XNN:  // VX += NN
        {
            V(c.x) += c.n & 0xFF;
            break;
        }

        case I_8XY0:  // VX = VY
        {
            V(c.x) = V(c.y);
            break;
        }
        case I_8XY1:  // VX |= VY
        {
            V(c.x) |= V(c.y);
            break;
        }
        case I_8XY2:  // VX &= VY
        {
            V(c.x) &= V(c.y);
            break;
        }
        case I_8XY3:  // VX ^= VY
        {
            V(c.x) ^= V(c.y);
            break;
        }

        case I_8XY4:  // VX += VY
        {
            if(V(c.x) + V(c.y) > 0xFF) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) += V(c.y);
            break;
        }
        case I_8XY5:  // VX -= VY
        {
            if(V(c.x) >= V(c.y)) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) -= V(c.y);
            break;
        }
        case I_8XY6:  // VX >>= VY
        {
            V(0xF) = V(c.x) & 0x1; // LSB
            V(c.x) >>= V(c.y);
            break;
        }
        case I_8XY7:  // VX = VY - VX
        {
            if(V(c.y) >= V(c.x)) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) = V(c.y) - V(c.x);
            break;
        }
        case I_8XYE:  // VX <<= VY
        {
            V(0xF) = (V(c.x) >> 7) & 0x1; // MSB
            V(c.x) <<= V(c.y);
            break;
        }

        case I_9XY0:  // if VX == VY then
        {
            if(V(c.x) != V(c.y)) skip_next = 1;
            break;
        }

        case I_ANNN:  // I = NNN
        {
            I = c.n;
            break;
        }
        case I_BNNN:  // jump0 + NNN
        {
            pc = V(0) + c.n;
            break;
        }

        case I_CXNN:  // VX = rand() & NN
        {
            V(c.x) = rand() & (c.n & 0xFF);
            break;
        }
        case I_DXYN:  // sprite VX VY N
        {
            assert(0 && "TODO: I_DXYN - Not implemented");
        }

        case I_EX9E:  // if VX -key then
            assert(0 && "TODO: I_EX9E - Not implemented");
        case I_EXA1:  // if VX key then
            assert(0 && "TODO: I_EXA1 - Not implemented");

        case I_FX07:  // VX = delay_timer
        {
            V(c.x) = delay_timer;
            break;
        }
        case I_FX0A:  // VX = key
            assert(0 && "TODO: I_FX0A - Not implemented");
        case I_FX15:  // delay_timer = VX
        {
            delay_timer = V(c.x);
            break;
        }
        case I_FX18:  // sound_timer = VX
        {
            sound_timer = V(c.x);
            break;
        }

        case I_FX1E:  // I += VX
        {
            I += V(c.x);
            break;
        }
        case I_FX29:  // I = hex VX
            assert(0 && "TODO: I_FX29 - Not implemented");

        case I_FX33:  // bcd VX
            assert(0 && "TODO: I_FX33 - Not implemented");
        case I_FX55:  // save VX
        {
            for(int i = 0; i <= c.x; i++) {
                memory[I + i] = V(i);
            }
            break;
        }
        case I_FX65:  // load VX
        {
            for(int i = 0; i <= c.x; i++) {
                V(i) = memory[I + i];
            }
            break;
        }

        default:
        {
            printf("Unknown instruction: %d\n", c.type);
            break;
        }
    }
}

int main() {
    command_opcode_debug(0x8A76);
    command_opcode_debug(0x203F);
    command_opcode_debug(0x2A3F);
    command_opcode_debug(0x3FA3);
    command_opcode_debug(0x3000);
    command_opcode_debug(0x8A23);

    return 0;
}
