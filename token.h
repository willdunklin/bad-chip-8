#ifndef CHIP8_TOKEN_H
#define CHIP8_TOKEN_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// Commands:
// call addr     |  O_2NNN  |  0x2FFF
// cls           |  O_00E0  |  0x00E0
// drw  Vx Vy N  |  O_DXYN  |  0xD01F   (VF = 1 on collision)
// jmp  addr     |  O_1NNN  |  0x1FFF
// jmp0 addr     |  O_BNNN  |  0xBFFF
// mov  Vx byte  |  O_6XNN  |  0x60FF
// mov  Vx Vy    |  O_8XY0  |  0x8010
// mov  Vx DT    |  O_FX07  |  0xF007
// mov  Vx K     |  O_FX0A  |  0xF00A
// mov  Vx [I]   |  O_FX65  |  0xF065
// mov  I addr   |  O_ANNN  |  0xAFFF
// mov  DT Vx    |  O_FX15  |  0xF015
// mov  ST Vx    |  O_FX18  |  0xF018
// mov  F Vx     |  O_FX29  |  0xF029
// mov  B Vx     |  O_FX33  |  0xF033
// mov  [I] Vx   |  O_FX55  |  0xF055
// rnd  Vx byte  |  O_CXNN  |  0xC0FF
// ret           |  O_00EE  |  0x00EE
// se   Vx byte  |  O_3XNN  |  0x30FF
// se   Vx Vy    |  O_5XY0  |  0x5010
// sne  Vx byte  |  O_4XNN  |  0x40FF
// sne  Vx Vy    |  O_9XY0  |  0x9010
// skp  Vx       |  O_EX9E  |  0xE09E
// sknp Vx       |  O_EXA1  |  0xE0A1
// add  Vx byte  |  O_7XNN  |  0x70FF
// add  Vx Vy    |  O_8XY4  |  0x8014   (VF = 1 on carry)
// add  I Vx     |  O_FX1E  |  0xF01E
// sub  Vx Vy    |  O_8XY5  |  0x8015   (VF = NOT borrow)
// subn Vx Vy    |  O_8XY7  |  0x8017   (VF = NOT borrow)
// and  Vx Vy    |  O_8XY2  |  0x8012
// or   Vx Vy    |  O_8XY1  |  0x8011
// xor  Vx Vy    |  O_8XY3  |  0x8013
// shr  Vx       |  O_8XY6  |  0x8016   (VF = LSB)
// shl  Vx       |  O_8XYE  |  0x801E   (VF = MSB)

// opcode to enum value substitution: X->0, Y->1, N->F
// - look at name-of-enum vs enum-value to translate
//   - example: O_8XY3 -> 0x8013, O_DXYN -> 0xD01F
typedef enum {
    O_00E0 = 0x00E0, // clear
    O_00EE = 0x00EE, // return            (exit subroutine)
    O_1NNN = 0x1FFF, // jump NNN
    O_2NNN = 0x2FFF, // NNN               (call subroutine)
    O_3XNN = 0x30FF, // if VX != NN then
    O_4XNN = 0x40FF, // if VX == NN then
    O_5XY0 = 0x5010, // if VX != VY then
    O_6XNN = 0x60FF, // VX = NN
    O_7XNN = 0x70FF, // VX += NN
    O_8XY0 = 0x8010, // VX = VY
    O_8XY1 = 0x8011, // VX |= VY          (bitwise OR)
    O_8XY2 = 0x8012, // VX &= VY          (bitwise AND)
    O_8XY3 = 0x8013, // VX ^= VY          (bitwise XOR)
    O_8XY4 = 0x8014, // VX += VY          (VF = 1 on carry)
    O_8XY5 = 0x8015, // VX -= VY          (VF = 0 on borrow)
    O_8XY6 = 0x8016, // VX >>= VY         (VF = LSB)
    O_8XY7 = 0x8017, // VX -= VY          (VF = 0 on borrow)
    O_8XYE = 0x801E, // VX <<= VY         (VF = MSB)
    O_9XY0 = 0x9010, // if VX == VY then
    O_ANNN = 0xAFFF, // I = NNN
    O_BNNN = 0xBFFF, // jump0 + NNN
    O_CXNN = 0xC0FF, // VX = rand() & NN  (rand <- [0, 255])
    O_DXYN = 0xD01F, // sprite VX VY N    (draw sprite, VF = 1 on collision)
    O_EX9E = 0xE09E, // if VX -key then   (is a key not pressed)
    O_EXA1 = 0xE0A1, // if VX key then    (is a key pressed)
    O_FX07 = 0xF007, // VX = delay_timer
    O_FX0A = 0xF00A, // VX = key          (wait for key press)
    O_FX15 = 0xF015, // delay_timer = VX
    O_FX18 = 0xF018, // sound_timer = VX
    O_FX1E = 0xF01E, // I += VX
    O_FX29 = 0xF029, // I = hex VX        (set I to a hex character)
    O_FX33 = 0xF033, // bcd VX            (decode VX into binary coded decimal)
    O_FX55 = 0xF055, // save VX           (save V0-VX to memory[I:I+X])
    O_FX65 = 0xF065  // load VX           (load V0-VX from memory[I:I+X])
} OpcodeType;

// Tokens:
// - call
// - cls
// - drw
// - jmp
// - jmp0
// - mov
// - rnd
// - ret
// - se
// - sne
// - skp
// - sknp
// - add
// - sub
// - subn
// - and
// - or
// - xor
// - shr
// - shl
//   Possible arguments:
//   - [I]
//   - I
//   - K
//   - B
//   - DT
//   - ST
//   - F
//   - Vx
//   - addr/byte/N -> int
typedef enum {
    T_INVALID=0,
    // Instructions
    T_CALL,
    T_CLS,
    T_DRW,
    T_JMP,
    T_JMP0,
    T_MOV,
    T_RND,
    T_RET,
    T_SE,
    T_SNE,
    T_SKP,
    T_SKNP,
    T_ADD,
    T_SUB,
    T_SUBN,
    T_AND,
    T_OR,
    T_XOR,
    T_SHR,
    T_SHL,
    // Literals
    T_VX,
    T_I,
    T_ADDR_I,
    T_K,
    T_B,
    T_DT,
    T_ST,
    T_F,
    // Extra
    T_NUM,
} Literal;

typedef struct {
    Literal literal;
    int value;
} Token;

typedef struct {
    uint16_t opcode;
    uint8_t arg_count;
    Token args[4];
} Instruction;


char* token_next(char* line) {
    while (*line != ' ' && *line != '\0') {
        line++;
    }
    while (*line == ' ') {
        *line = '\0';
        line++;
    }
    return line;
}

Instruction token_extract_from_line(char* line) {
    Instruction instruction = {0};

    char* str = (char*)malloc(2048); // assuming that lines will be <2048 chars ((probably))
    char* start = str; // hold on to pointer to free later
    memset(str, 0, 2048);
    strcpy(str, line);

    // remove leading spaces
    while(*str == ' ') str++;

    while (*str != '\0') {
        Token* token = &instruction.args[instruction.arg_count++];

        // skips non-spaces, fills trailing spaces with \0, returns pointer to start of next token
        char* rest = token_next(str);

        // printf("str: %s\n", str);

        if (strcmp(str, "call") == 0)      token->literal = T_CALL;
        else if (strcmp(str, "cls") == 0)  token->literal = T_CLS;
        else if (strcmp(str, "drw") == 0)  token->literal = T_DRW;
        else if (strcmp(str, "jmp") == 0)  token->literal = T_JMP;
        else if (strcmp(str, "jmp0") == 0) token->literal = T_JMP0;
        else if (strcmp(str, "mov") == 0)  token->literal = T_MOV;
        else if (strcmp(str, "rnd") == 0)  token->literal = T_RND;
        else if (strcmp(str, "ret") == 0)  token->literal = T_RET;
        else if (strcmp(str, "se") == 0)   token->literal = T_SE;
        else if (strcmp(str, "sne") == 0)  token->literal = T_SNE;
        else if (strcmp(str, "skp") == 0)  token->literal = T_SKP;
        else if (strcmp(str, "sknp") == 0) token->literal = T_SKNP;
        else if (strcmp(str, "add") == 0)  token->literal = T_ADD;
        else if (strcmp(str, "sub") == 0)  token->literal = T_SUB;
        else if (strcmp(str, "subn") == 0) token->literal = T_SUBN;
        else if (strcmp(str, "and") == 0)  token->literal = T_AND;
        else if (strcmp(str, "or") == 0)   token->literal = T_OR;
        else if (strcmp(str, "xor") == 0)  token->literal = T_XOR;
        else if (strcmp(str, "shr") == 0)  token->literal = T_SHR;
        else if (strcmp(str, "shl") == 0)  token->literal = T_SHL;
        else if (strcmp(str, "I") == 0)    token->literal = T_I;
        else if (strcmp(str, "[I]") == 0)  token->literal = T_ADDR_I;
        else if (strcmp(str, "K") == 0)    token->literal = T_K;
        else if (strcmp(str, "B") == 0)    token->literal = T_B;
        else if (strcmp(str, "DT") == 0)   token->literal = T_DT;
        else if (strcmp(str, "ST") == 0)   token->literal = T_ST;
        else if (strcmp(str, "F") == 0)    token->literal = T_F;
        else {
            if (str[0] == 'V') {
                token->literal = T_VX;
                // by masking the token->value for Vx now, we can use it safely later
                token->value = strtol(str + 1, NULL, 16) & 0xF;
            } else {
                token->literal = T_NUM;
                token->value = atoi(str);
            }
        }
        str = rest;
    }
    free(start);
    return instruction;
}

Instruction token_parse_line(char* line) {
    // returns an instruction with tokens extracted into args
    Instruction ins = token_extract_from_line(line);

    Token* op = ins.args;

    switch (op[0].literal) {
        case T_INVALID: {
            // assert(op[0].literal != T_INVALID && "Invalid starting token");
            // break;
            return ins;
        }
        case T_CALL: {
            assert(ins.arg_count == 2 && "Invalid number of arguments for 'call'");
            assert(op[1].literal == T_NUM && "Invalid argument type for 'call'");
            ins.opcode = 0x2000 | (op[1].value & 0x0FFF);
            break;
        }
        case T_CLS: {
            assert(ins.arg_count == 1 && "Invalid number of arguments for 'cls'");
            ins.opcode = 0x00E0;
            break;
        }
        case T_DRW: {
            assert(ins.arg_count == 4 && "Invalid number of arguments for 'drw'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'drw'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'drw'");
            assert(op[3].literal == T_NUM && "Invalid argument type for 'drw'");
            ins.opcode = 0xD000 | (op[1].value << 8) | (op[2].value << 4) | (op[3].value & 0xF);
            break;
        }
        case T_JMP: {
            assert(ins.arg_count == 2 && "Invalid number of arguments for 'jmp'");
            assert(op[1].literal == T_NUM && "Invalid argument type for 'jmp'");
            ins.opcode = 0x1000 | (op[1].value & 0x0FFF);
            break;
        }
        case T_JMP0: {
            assert(ins.arg_count == 2 && "Invalid number of arguments for 'jmp0'");
            assert(op[1].literal == T_NUM && "Invalid argument type for 'jmp0'");
            ins.opcode = 0xB000 | (op[1].value & 0x0FFF);
            break;
        }
        case T_MOV: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'mov'");

            if (op[1].literal == T_VX) {
                switch (op[2].literal) {
                    // mov Vx byte
                    case T_NUM: {
                        ins.opcode = 0x6000 | (op[1].value << 8) | (op[2].value & 0xFF);
                        break;
                    }
                    // mov Vx Vy
                    case T_VX: {
                        ins.opcode = 0x8000 | (op[1].value << 8) | (op[2].value << 4);
                        break;
                    }
                    // mov Vx DT
                    case T_DT: {
                        ins.opcode = 0xF007 | (op[1].value << 8);
                        break;
                    }
                    // mov Vx K
                    case T_K: {
                        ins.opcode = 0xF00A | (op[1].value << 8);
                        break;
                    }
                    // mov Vx [I]
                    case T_ADDR_I: {
                        ins.opcode = 0xF065 | (op[1].value << 8);
                        break;
                    }
                    default: assert(op[2].literal == T_INVALID && "Invalid argument type for 'mov'");
                }
            } else if (op[1].literal == T_I) {
                assert(op[2].literal == T_NUM && "Invalid argument type for 'mov'");
                ins.opcode = 0xA000 | (op[2].value & 0x0FFF);

            // test based on the second arg (all the rest should have a V register)
            } else if (op[2].literal == T_VX) {
                switch (op[1].literal) {
                    // mov DT Vx
                    case T_DT: {
                        ins.opcode = 0xF015 | (op[2].value << 8);
                        break;
                    }
                    // mov ST Vx
                    case T_ST: {
                        ins.opcode = 0xF018 | (op[2].value << 8);
                        break;
                    }
                    // mov F Vx
                    case T_F: {
                        ins.opcode = 0xF029 | (op[2].value << 8);
                        break;
                    }
                    // mov B Vx
                    case T_B: {
                        ins.opcode = 0xF033 | (op[2].value << 8);
                        break;
                    }
                    // mov [I] Vx
                    case T_ADDR_I: {
                        ins.opcode = 0xF055 | (op[2].value << 8);
                        break;
                    }
                    default: assert(op[1].literal == T_INVALID && "Invalid argument type for 'mov'");
                }
            } else {
                assert(op[1].literal == T_INVALID && "Invalid argument type for 'mov'");
            }
            break;
        }
        case T_RND: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'rnd'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'rnd'");
            assert(op[2].literal == T_NUM && "Invalid argument type for 'rnd'");
            ins.opcode = 0xC000 | (op[1].value << 8) | (op[2].value & 0xFF);
            break;
        }
        case T_RET: {
            assert(ins.arg_count == 1 && "Invalid number of arguments for 'ret'");
            ins.opcode = 0x00EE;
            break;
        }
        case T_SE: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'se'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'se'");
            // se Vx Vy
            if (op[2].literal == T_VX) {
                ins.opcode = 0x5000 | (op[1].value << 8) | (op[2].value << 4);
            // se Vx byte
            } else if (op[2].literal == T_NUM) {
                ins.opcode = 0x3000 | (op[1].value << 8) | (op[2].value & 0xFF);
            } else {
                assert(op[2].literal == T_INVALID && "Invalid argument type for 'se'");
            }
            break;
        }
        case T_SNE: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'sne'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'sne'");
            // sne Vx Vy
            if (op[2].literal == T_VX) {
                ins.opcode = 0x9000 | (op[1].value << 8) | (op[2].value << 4);
            // sne Vx byte
            } else if (op[2].literal == T_NUM) {
                ins.opcode = 0x4000 | (op[1].value << 8) | (op[2].value & 0xFF);
            } else {
                assert(op[2].literal == T_INVALID && "Invalid argument type for 'se'");
            }
            break;
        }
        case T_SKP: {
            assert(ins.arg_count == 2 && "Invalid number of arguments for 'skp'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'skp'");
            ins.opcode = 0xE09E | (op[1].value << 8);
            break;
        }
        case T_SKNP: {
            assert(ins.arg_count == 2 && "Invalid number of arguments for 'sknp'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'sknp'");
            ins.opcode = 0xE0A1 | (op[1].value << 8);
            break;
        }
        case T_ADD: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'add'");
            if (op[1].literal == T_VX) {
                // add Vx Vy
                if (op[2].literal == T_VX) {
                    ins.opcode = 0x8004 | (op[1].value << 8) | (op[2].value << 4);
                // add Vx byte
                } else if (op[2].literal == T_NUM) {
                    ins.opcode = 0x7000 | (op[1].value << 8) | (op[2].value & 0xFF);
                } else {
                    assert(op[2].literal == T_INVALID && "Invalid argument type for 'add'");
                }
            // add I Vx
            } else if (op[1].literal == T_I) {
                ins.opcode = 0xF01E | (op[2].value << 8);
            } else {
                assert(op[1].literal == T_INVALID && "Invalid argument type for 'add'");
            }
            break;
        }
        case T_SUB: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'sub'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'sub'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'sub'");
            ins.opcode = 0x8005 | (op[1].value << 8) | (op[2].value << 4);
            break;
        }
        case T_SUBN: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'subn'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'subn'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'subn'");
            ins.opcode = 0x8007 | (op[1].value << 8) | (op[2].value << 4);
            break;
        }
        case T_AND: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'and'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'and'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'and'");
            ins.opcode = 0x8002 | (op[1].value << 8) | (op[2].value << 4);
            break;
        }
        case T_OR: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'or'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'or'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'or'");
            ins.opcode = 0x8001 | (op[1].value << 8) | (op[2].value << 4);
            break;
        }
        case T_XOR: {
            assert(ins.arg_count == 3 && "Invalid number of arguments for 'xor'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'xor'");
            assert(op[2].literal == T_VX && "Invalid argument type for 'xor'");
            ins.opcode = 0x8003 | (op[1].value << 8) | (op[2].value << 4);
            break;
        }
        case T_SHR: {
            assert(ins.arg_count >= 2 && "Invalid number of arguments for 'shr'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'shr'");
            ins.opcode = 0x8006 | (op[1].value << 8);
            break;
        }
        case T_SHL: {
            assert(ins.arg_count >= 2 && "Invalid number of arguments for 'shl'");
            assert(op[1].literal == T_VX && "Invalid argument type for 'shl'");
            ins.opcode = 0x800E | (op[1].value << 8);
            break;
        }
        default: assert(op[0].literal == T_INVALID && "Invalid starting token");
    }

    return ins;
}

#endif //CHIP8_TOKEN_H
