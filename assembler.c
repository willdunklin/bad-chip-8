#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <byteswap.h>

#include "instruction.h"
#include "command.h"
#include "token.h"
#include "util.h"

void print_inst(Instruction ins)
{
    printf("Instruction: 0x%X | #%d:\n", ins.opcode, ins.arg_count);
    for (int i = 0; i < ins.arg_count; i++) {
        printf("  arg[%d]: %d, 0x%X\n", i, ins.args[i].literal, ins.args[i].value);
    }
    printf("\n");
}

void test() {
    print_inst(token_parse_line("jmp    0"));
    print_inst(token_parse_line("jmp 1293"));
    print_inst(token_parse_line("ret  "));
    print_inst(token_parse_line("  ret"));
    print_inst(token_parse_line("drw V0 VF 39"));
    print_inst(token_parse_line("drw V0 VF 7"));
    print_inst(token_parse_line("add I V2"));
    print_inst(token_parse_line("mov Va [I] v9"));
}


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <input-asm> <output-bin>\n", argv[0]);
        return 1;
    }

    String input = {0};
    if (!util_read_file(argv[1], &input)) {
        printf("Error: Could not read file: %s\n", argv[1]);
        return 1;
    }

    Opcodes ops = {0};
    CString_List lines = {0};

    int line_idx = 0;
    char* line = strtok(input.items, "\n");

    while (line != NULL) {
        Instruction ins = token_parse_line(line);
        if (ins.opcode == 0) {
            printf("Warning: Could not parse line: %d\n", line_idx);
        } else {
            uint16_t op = __bswap_16(ins.opcode); // swap endian-ness for big-endian in file
            util_da_append(&ops, op);
        }
        util_da_append(&lines, line);
        line = strtok(NULL, "\n");
        line_idx++;
    }

    printf("\nParsed Lines:\n");
    for (int i = 0; i < lines.count; i++) {
        printf("%02d: %s\n", i, lines.items[i]);
    }

    printf("\nOpcodes:\n");
    for (int i = 0; i < ops.count; i++) {
        uint16_t op = __bswap_16(ops.items[i]); // swap endian-ness back for printing lol
        printf("%04X\n", op);
    }

    // create binary with UTIL_INSTRUCTION_START offset
    int bin_length = UTIL_INSTRUCTION_START + ops.count * 2;
    uint8_t* binary = malloc(bin_length);

    memset(binary, 0, UTIL_INSTRUCTION_START);
    memcpy(binary + UTIL_INSTRUCTION_START, ops.items, ops.count * 2);

    if (!util_write_file(argv[2], binary, bin_length)) {
        printf("Error: Could not write file: %s\n", argv[2]);
        return 1;
    }

    return 0;
}
