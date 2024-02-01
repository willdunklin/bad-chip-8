# CHIP8 Attempt

## Quick Start

```bash
cmake -B ./build -G Ninja
cmake --build ./build && ./build/chip8

# to try assembler run the following binary (after building of course)
./build/chip8asm ./test/foo.asm ./test/bar.bin
```

## Resources

- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [Instruction Set Reference](https://johnearnest.github.io/Octo/docs/chip8ref.pdf)
