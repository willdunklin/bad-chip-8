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

uint16_t line_to_instruction_type(char* line) {
    Instruction ins = token_parse_line(line);
    Command cmd = command_parse_opcode(ins.opcode);
    // printf("Ins: %04X\n", ins.opcode);
    // printf("Cmd: %04X\n\n", cmd.type);
    return cmd.type;
}

void test_all_codes() {
    assert(line_to_instruction_type("call 342")     == 0x2FFF);
    assert(line_to_instruction_type("cls")          == 0x00E0);
    assert(line_to_instruction_type("drw  V1 V2 3") == 0xD01F);
    assert(line_to_instruction_type("jmp  1000")    == 0x1FFF);
    assert(line_to_instruction_type("jmp0 3494")    == 0xBFFF);
    assert(line_to_instruction_type("mov  V3 250")  == 0x60FF);
    assert(line_to_instruction_type("mov  V1 V2")   == 0x8010);
    assert(line_to_instruction_type("mov  V3 DT")   == 0xF007);
    assert(line_to_instruction_type("mov  V2 K")    == 0xF00A);
    assert(line_to_instruction_type("mov  V4 [I]")  == 0xF065);
    assert(line_to_instruction_type("mov  I 496")   == 0xAFFF);
    assert(line_to_instruction_type("mov  DT V7")   == 0xF015);
    assert(line_to_instruction_type("mov  ST V8")   == 0xF018);
    assert(line_to_instruction_type("mov  F V5")    == 0xF029);
    assert(line_to_instruction_type("mov  B V9")    == 0xF033);
    assert(line_to_instruction_type("mov  [I] Va")  == 0xF055);
    assert(line_to_instruction_type("rnd  V0 13")   == 0xC0FF);
    assert(line_to_instruction_type("ret")          == 0x00EE);
    assert(line_to_instruction_type("se   V9 131")  == 0x30FF);
    assert(line_to_instruction_type("se   V3 V4")   == 0x5010);
    assert(line_to_instruction_type("sne  V6 70")   == 0x40FF);
    assert(line_to_instruction_type("sne  V6 V7")   == 0x9010);
    assert(line_to_instruction_type("skp  Vb")      == 0xE09E);
    assert(line_to_instruction_type("sknp Ve")      == 0xE0A1);
    assert(line_to_instruction_type("add  Vc 96")   == 0x70FF);
    assert(line_to_instruction_type("add  Vd Ve")   == 0x8014);
    assert(line_to_instruction_type("add  I V3")    == 0xF01E);
    assert(line_to_instruction_type("sub  Vd V2")   == 0x8015);
    assert(line_to_instruction_type("subn V4 V3")   == 0x8017);
    assert(line_to_instruction_type("and  V6 V6")   == 0x8012);
    assert(line_to_instruction_type("or   V7 V5")   == 0x8011);
    assert(line_to_instruction_type("xor  V8 V2")   == 0x8013);
    assert(line_to_instruction_type("shr  V0")      == 0x8016);
    assert(line_to_instruction_type("shl  V9")      == 0x801E);
}


int main(int argc, char** argv) {
    test_all_codes();

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

    free(binary);
    return 0;
}
