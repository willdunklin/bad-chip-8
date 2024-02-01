#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "command.h"
#include "display.h"
#include "util.h"

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
    printf("V[0-2]: %02X, %02X, %02X\n", registers[0], registers[1], registers[2]);
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1]; // read big-endian 16-bit opcode
    printf("Opcode: %04X\n", opcode);
    Command c = command_parse_opcode(opcode);

    switch(c.type) {
        // cls
        case O_00E0: {
            clear_display();
            break;
        }
        // ret
        case O_00EE: {
            pc = stack[sp];
            sp -= 1;
            sp = (sp + 16) & 0xF; // wrap around
            break;
        }
        // jmp nnn
        case O_1NNN: {
            pc = c.n - 2; // -2 because pc is incremented at end of step
            break;
        }
        // call nnn
        case O_2NNN: {
            sp += 1;
            sp = (sp + 16) & 0xF; // wrap around
            stack[sp] = pc;
            pc = c.n;
            break;
        }

        // se Vx nn
        case O_3XNN: {
            if(registers[c.x] == (c.n & 0xFF)) pc += 2;
            break;
        }
        // sne Vx nn
        case O_4XNN: {
            if(registers[c.x] != (c.n & 0xFF)) pc += 2;
            break;
        }
        // se Vx Vy
        case O_5XY0: {
            if(registers[c.x] != registers[c.y])       pc += 2;
            break;
        }

        // mov Vx nn
        case O_6XNN: {
            registers[c.x] = c.n & 0xFF;
            break;
        }
        // add Vx nn
        case O_7XNN: {
            registers[c.x] += c.n & 0xFF;
            break;
        }

        // mov Vx Vy
        case O_8XY0: {
            registers[c.x] = registers[c.y];
            break;
        }
        // or Vx Vy
        case O_8XY1: {
            registers[c.x] |= registers[c.y];
            break;
        }
        // and Vx Vy
        case O_8XY2: {
            registers[c.x] &= registers[c.y];
            break;
        }
        // xor Vx Vy
        case O_8XY3: {
            registers[c.x] ^= registers[c.y];
            break;
        }

        // add Vx Vy  (VF = 1 on carry)
        case O_8XY4: {
            if(registers[c.x] + registers[c.y] > 0xFF) {
                registers[0xF] = 1;
            } else {
                registers[0xF] = 0;
            }
            registers[c.x] += registers[c.y];
            break;
        }
        // sub Vx Vy  (VF = 0 on borrow)
        case O_8XY5: {
            if(registers[c.x] >= registers[c.y]) {
                registers[0xF] = 1;
            } else {
                registers[0xF] = 0;
            }
            registers[c.x] -= registers[c.y];
            break;
        }
        // shr Vx  (VF = LSB)
        case O_8XY6: {
            registers[0xF] = registers[c.x] & 0x1; // LSB
            registers[c.x] >>= 1;
            break;
        }
        // subn Vx Vy  (VF = 0 on borrow)
        case O_8XY7: {
            if(registers[c.y] >= registers[c.x]) {
                registers[0xF] = 1;
            } else {
                registers[0xF] = 0;
            }
            registers[c.x] = registers[c.y] - registers[c.x];
            break;
        }
        // shl Vx  (VF = MSB)
        case O_8XYE: {
            registers[0xF] = (registers[c.x] >> 7) & 0x1; // MSB
            registers[c.x] <<= 1;
            break;
        }

        // sne Vx Vy
        case O_9XY0: {
            if(registers[c.x] != registers[c.y]) pc += 2;
            break;
        }

        // mov I nnn
        case O_ANNN: {
            I = c.n;
            break;
        }
        // jmp0 nnn
        case O_BNNN: {
            pc = registers[0] + c.n - 2; // -2 because pc is incremented at end of step
            break;
        }

        // rnd Vx nn
        case O_CXNN: {
            registers[c.x] = rand() & (c.n & 0xFF);
            break;
        }
        // drw Vx Vy n
        case O_DXYN: {
            assert(0 && "TODO: O_DXYN - Not implemented");
        }

        // skp Vx
        case O_EX9E:
            assert(0 && "TODO: O_EX9E - Not implemented");
        // sknp Vx
        case O_EXA1:
            assert(0 && "TODO: O_EXA1 - Not implemented");

        // mov Vx DT
        case O_FX07: {
            registers[c.x] = delay_timer;
            break;
        }
        // mov Vx K
        case O_FX0A:
            assert(0 && "TODO: O_FX0A - Not implemented");
        // mov DT Vx
        case O_FX15: {
            delay_timer = registers[c.x];
            break;
        }
        // mov ST Vx
        case O_FX18: {
            sound_timer = registers[c.x];
            break;
        }

        // add I Vx
        case O_FX1E: {
            I += registers[c.x];
            break;
        }
        // mov I Vx
        case O_FX29:
            assert(0 && "TODO: O_FX29 - Not implemented");

        // mov B Vx
        case O_FX33:
            assert(0 && "TODO: O_FX33 - Not implemented");
        // mov [I] Vx
        case O_FX55: {
            for(int i = 0; i <= c.x; i++) {
                memory[I + i] = registers[i];
            }
            break;
        }
        // mov Vx [I]
        case O_FX65: {
            for(int i = 0; i <= c.x; i++) {
                registers[i] = memory[I + i];
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
