# Assembly Spec

As mentioned in the readme, the assembler is very simple. It ignores spaces, blank lines and nothing else. These are the commands that are supported:

```
   command    |  instr.  |  opcode
------------- | -------- | --------
call addr     |  O_2NNN  |  0x2FFF
cls           |  O_00E0  |  0x00E0
drw  Vx Vy N  |  O_DXYN  |  0xD01F   (VF = 1 on collision)
jmp  addr     |  O_1NNN  |  0x1FFF
jmp0 addr     |  O_BNNN  |  0xBFFF
mov  Vx byte  |  O_6XNN  |  0x60FF
mov  Vx Vy    |  O_8XY0  |  0x8010
mov  Vx DT    |  O_FX07  |  0xF007
mov  Vx K     |  O_FX0A  |  0xF00A
mov  Vx [I]   |  O_FX65  |  0xF065
mov  I addr   |  O_ANNN  |  0xAFFF
mov  DT Vx    |  O_FX15  |  0xF015
mov  ST Vx    |  O_FX18  |  0xF018
mov  F Vx     |  O_FX29  |  0xF029
mov  B Vx     |  O_FX33  |  0xF033
mov  [I] Vx   |  O_FX55  |  0xF055
rnd  Vx byte  |  O_CXNN  |  0xC0FF
ret           |  O_00EE  |  0x00EE
se   Vx byte  |  O_3XNN  |  0x30FF
se   Vx Vy    |  O_5XY0  |  0x5010
sne  Vx byte  |  O_4XNN  |  0x40FF
sne  Vx Vy    |  O_9XY0  |  0x9010
skp  Vx       |  O_EX9E  |  0xE09E
sknp Vx       |  O_EXA1  |  0xE0A1
add  Vx byte  |  O_7XNN  |  0x70FF
add  Vx Vy    |  O_8XY4  |  0x8014   (VF = 1 on carry)
add  I Vx     |  O_FX1E  |  0xF01E
sub  Vx Vy    |  O_8XY5  |  0x8015   (VF = NOT borrow)
subn Vx Vy    |  O_8XY7  |  0x8017   (VF = NOT borrow)
and  Vx Vy    |  O_8XY2  |  0x8012
or   Vx Vy    |  O_8XY1  |  0x8011
xor  Vx Vy    |  O_8XY3  |  0x8013
shr  Vx       |  O_8XY6  |  0x8016   (VF = LSB)
shl  Vx       |  O_8XYE  |  0x801E   (VF = MSB)
```

This generally follows the syntax of [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM), however the following are renamed: `ld` -> `mov` and `jp` -> `jmp`. Also I split off the opcode `Bnnn` (another jump instruction), from the syntax `jmp V0, addr` to `jmp0 addr`, because it wasn't worth a headache at the time.

Since comments aren't supported I decided to kill commas as well; don't try to use them in your assembly :)
