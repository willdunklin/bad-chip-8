# Questionable (C)HIP-8

## Quick Start

```bash
cmake -B ./build -G Ninja  # -G Ninja is optional but recommended
cmake --build ./build && ./build/chip8

# to try assembler run the following binary (after building of course)
./build/chip8asm ./test/foo.asm ./test/bar.bin
```

### Build, Assbemble and Run
```bash
clear; cmake --build ./build && ./build/chip8asm test/foo.asm test/bar.bin > /dev/null && ./build/chip8 test/bar.bin
```

## Assembly Info

I've added a simple assembler (but don't try to push it very far). It ignores spaces, blank lines and nothing else. The basic syntax is as follows:

```asm
mov  v0 2
jmp0 198
add  v0 60
```

See more in [assembly.md](./assembly.md).

## Resources

- [Cowgod's Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [CHIP-8 Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [Instruction Set Reference](https://johnearnest.github.io/Octo/docs/chip8ref.pdf)
- [CMake Docs](https://cmake.org/cmake/help/latest/index.html)
- [CMake Download](https://cmake.org/download/)
