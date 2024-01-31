#ifndef CHIP8_INSTRUCTION_H
#define CHIP8_INSTRUCTION_H

// opcode to enum value substitution: X->0, Y->1, N->F
// - look at name-of-enum vs enum-value to translate
//   - example: I_8XY3 -> 0x8013, I_DXYN -> 0xD01F
typedef enum {
    I_00E0 = 0x00E0, // clear
    I_00EE = 0x00EE, // return            (exit subroutine)
    I_1NNN = 0x1FFF, // jump NNN
    I_2NNN = 0x2FFF, // NNN               (call subroutine)
    I_3XNN = 0x30FF, // if VX != NN then
    I_4XNN = 0x40FF, // if VX == NN then
    I_5XY0 = 0x5010, // if VX != VY then
    I_6XNN = 0x60FF, // VX = NN
    I_7XNN = 0x70FF, // VX += NN
    I_8XY0 = 0x8010, // VX = VY
    I_8XY1 = 0x8011, // VX |= VY          (bitwise OR)
    I_8XY2 = 0x8012, // VX &= VY          (bitwise AND)
    I_8XY3 = 0x8013, // VX ^= VY          (bitwise XOR)
    I_8XY4 = 0x8014, // VX += VY          (VF = 1 on carry)
    I_8XY5 = 0x8015, // VX -= VY          (VF = 0 on borrow)
    I_8XY6 = 0x8016, // VX >>= VY         (VF = LSB)
    I_8XY7 = 0x8017, // VX -= VY          (VF = 0 on borrow)
    I_8XYE = 0x801E, // VX <<= VY         (VF = MSB)
    I_9XY0 = 0x9010, // if VX == VY then
    I_ANNN = 0xAFFF, // I = NNN
    I_BNNN = 0xBFFF, // jump0 + NNN
    I_CXNN = 0xC0FF, // VX = rand() & NN  (rand <- [0, 255])
    I_DXYN = 0xD01F, // sprite VX VY N    (draw sprite, VF = 1 on collision)
    I_EX9E = 0xE09E, // if VX -key then   (is a key not pressed)
    I_EXA1 = 0xE0A1, // if VX key then    (is a key pressed)
    I_FX07 = 0xF007, // VX = delay_timer
    I_FX0A = 0xF00A, // VX = key          (wait for key press)
    I_FX15 = 0xF015, // delay_timer = VX
    I_FX18 = 0xF018, // sound_timer = VX
    I_FX1E = 0xF01E, // I += VX
    I_FX29 = 0xF029, // I = hex VX        (set I to a hex character)
    I_FX33 = 0xF033, // bcd VX            (decode VX into binary coded decimal)
    I_FX55 = 0xF055, // save VX           (save V0-VX to memory[I:I+X])
    I_FX65 = 0xF065  // load VX           (load V0-VX from memory[I:I+X])
} instruction;

#endif // CHIP8_INSTRUCTION_H
