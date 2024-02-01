#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "instruction.h"
#include "command.h"
#include "display.h"
#include "util.h"

#define V(X)  registers[X]   // V0-VF (must use numeric index 0-15, 0x0-0xf)

uint8_t memory[4096] = {0};
uint16_t I = 0;              // index register (used for memory addresses)
uint16_t pc = 0x200;         // program counter (0x200 is presumed entrypoint)
uint8_t registers[16] = {0}; // V0-VF registers
uint16_t stack[16] = {0};    // stack
uint8_t sp = 0;              // stack pointer

uint8_t delay_timer; // TODO: decrement at 60hz
uint8_t sound_timer; // TODO: decrement at 60hz

void step() {
    printf("\n");
    printf("PC: %04X  |  I: %04X {%02X}\n", pc, I, memory[I]);
    printf("V[0-2]: %02X, %02X, %02X\n", V(0), V(1), V(2));
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1]; // read big-endian 16-bit opcode
    printf("Opcode: %04X\n", opcode);
    Command c = command_parse_opcode(opcode);

    switch(c.type) {
        // cls
        case I_00E0: {
            clear_display();
            break;
        }
        // ret
        case I_00EE: {
            pc = stack[sp];
            sp -= 1;
            sp = (sp + 16) & 0xF; // wrap around
            break;
        }
        // jmp nnn
        case I_1NNN: {
            pc = c.n - 2; // -2 because pc is incremented at end of step
            break;
        }
        // call nnn
        case I_2NNN: {
            sp += 1;
            sp = (sp + 16) & 0xF; // wrap around
            stack[sp] = pc;
            pc = c.n;
            break;
        }

        // se Vx nn
        case I_3XNN: {
            if(V(c.x) == (c.n & 0xFF)) pc += 2;
            break;
        }
        // sne Vx nn
        case I_4XNN: {
            if(V(c.x) != (c.n & 0xFF)) pc += 2;
            break;
        }
        // se Vx Vy
        case I_5XY0: {
            if(V(c.x) != V(c.y))       pc += 2;
            break;
        }

        // mov Vx nn
        case I_6XNN: {
            V(c.x) = c.n & 0xFF;
            break;
        }
        // add Vx nn
        case I_7XNN: {
            V(c.x) += c.n & 0xFF;
            break;
        }

        // mov Vx Vy
        case I_8XY0: {
            V(c.x) = V(c.y);
            break;
        }
        // or Vx Vy
        case I_8XY1: {
            V(c.x) |= V(c.y);
            break;
        }
        // and Vx Vy
        case I_8XY2: {
            V(c.x) &= V(c.y);
            break;
        }
        // xor Vx Vy
        case I_8XY3: {
            V(c.x) ^= V(c.y);
            break;
        }

        // add Vx Vy  (VF = 1 on carry)
        case I_8XY4: {
            if(V(c.x) + V(c.y) > 0xFF) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) += V(c.y);
            break;
        }
        // sub Vx Vy  (VF = 0 on borrow)
        case I_8XY5: {
            if(V(c.x) >= V(c.y)) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) -= V(c.y);
            break;
        }
        // shr Vx  (VF = LSB)
        case I_8XY6: {
            V(0xF) = V(c.x) & 0x1; // LSB
            V(c.x) >>= 1;
            break;
        }
        // subn Vx Vy  (VF = 0 on borrow)
        case I_8XY7: {
            if(V(c.y) >= V(c.x)) {
                V(0xF) = 1;
            } else {
                V(0xF) = 0;
            }
            V(c.x) = V(c.y) - V(c.x);
            break;
        }
        // shl Vx  (VF = MSB)
        case I_8XYE: {
            V(0xF) = (V(c.x) >> 7) & 0x1; // MSB
            V(c.x) <<= 1;
            break;
        }

        // sne Vx Vy
        case I_9XY0: {
            if(V(c.x) != V(c.y)) pc += 2;
            break;
        }

        // mov I nnn
        case I_ANNN: {
            I = c.n;
            break;
        }
        // jmp0 nnn
        case I_BNNN: {
            pc = V(0) + c.n - 2; // -2 because pc is incremented at end of step
            break;
        }

        // rnd Vx nn
        case I_CXNN: {
            V(c.x) = rand() & (c.n & 0xFF);
            break;
        }
        // drw Vx Vy n
        case I_DXYN: {
            assert(0 && "TODO: I_DXYN - Not implemented");
        }

        // skp Vx
        case I_EX9E:
            assert(0 && "TODO: I_EX9E - Not implemented");
        // sknp Vx
        case I_EXA1:
            assert(0 && "TODO: I_EXA1 - Not implemented");

        // mov Vx DT
        case I_FX07: {
            V(c.x) = delay_timer;
            break;
        }
        // mov Vx K
        case I_FX0A:
            assert(0 && "TODO: I_FX0A - Not implemented");
        // mov DT Vx
        case I_FX15: {
            delay_timer = V(c.x);
            break;
        }
        // mov ST Vx
        case I_FX18: {
            sound_timer = V(c.x);
            break;
        }

        // add I Vx
        case I_FX1E: {
            I += V(c.x);
            break;
        }
        // mov I Vx
        case I_FX29:
            assert(0 && "TODO: I_FX29 - Not implemented");

        // mov B Vx
        case I_FX33:
            assert(0 && "TODO: I_FX33 - Not implemented");
        // mov [I] Vx
        case I_FX55: {
            for(int i = 0; i <= c.x; i++) {
                memory[I + i] = V(i);
            }
            break;
        }
        // mov Vx [I]
        case I_FX65: {
            for(int i = 0; i <= c.x; i++) {
                V(i) = memory[I + i];
            }
            break;
        }

        case 0: {
            printf("nop: %d\n", c.type);
            break;
        }
        default: {
            printf("Unknown instruction: %d\n", c.type);
            assert(0 && "ERROR: Unknown instruction");
            break;
        }
    }

    pc += 2; // increment program counter one word
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <input-bin>\n", argv[0]);
        return 1;
    }

    String input = {0};
    if (!util_read_file(argv[1], &input)) {
        printf("Error: Could not read file: %s\n", argv[1]);
        return 1;
    }

    memcpy(memory, input.items, input.count);

    // just test a few steps for now
    for (int i = 0; i < 60; i++) {
        step();
    }

    return 0;
}
