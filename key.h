#ifndef CHIP8_KEY_H
#define CHIP8_KEY_H

#include <stdint.h>
#include <ncurses.h>
#include <stdbool.h>

int char_to_hex_val(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}

uint8_t get_hex_key_block() {
    int key;
    while (1) {
        key = char_to_hex_val(getch());
        if (key != -1)
            return key;
    }
}

int get_hex_key_timeout(int timeout_ms) {
    timeout(timeout_ms);
    int key = getch();
    timeout(0);
    if (key == ERR)
        return -1;

    return char_to_hex_val(key);
}

#endif //CHIP8_KEY_H
