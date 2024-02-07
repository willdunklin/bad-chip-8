#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <ncurses.h>
#include <stdio.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

uint8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT] = {0};

WINDOW* display_win;
WINDOW* debug_win;

uint8_t display_draw_sprite(uint8_t x, uint8_t y, uint8_t n, uint8_t *memory) {
    uint8_t collision = 0;
    // display n-byte sprite starting at memory (offset from I) at coordinates (Vx, Vy), set VF = pixel collision
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t* prev_pixel = &(display[(x + j) % DISPLAY_WIDTH][(y + i) % DISPLAY_HEIGHT]);
            uint8_t new_pixel = (memory[i] >> (7 - j)) & 1;

            if (!collision && (*prev_pixel) && new_pixel)
                collision = 1;

            (*prev_pixel) ^= new_pixel;
        }
    }
    return collision;
}

void display_init() {
    initscr();
    noecho();
    cbreak();

    display_win = newwin(DISPLAY_HEIGHT, DISPLAY_WIDTH, 0, 0);
    debug_win = newwin(DISPLAY_HEIGHT, DISPLAY_WIDTH + 20, 0, DISPLAY_WIDTH + 1);
    keypad(display_win, TRUE);
    nodelay(display_win, TRUE);
    wrefresh(display_win);
    wrefresh(debug_win);

    refresh();
}

void display_end() {
    endwin();
}

void display_refresh() {
    wmove(display_win, 0, 0);
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
            if (display[j][i])
                waddch(display_win, '0');
            else
                waddch(display_win, '.');
        }
    }
    wrefresh(display_win);
}

void display_clear() {
    memset(display, 0, sizeof(display));
    display_refresh();
}

void display_debug_info(uint16_t pc, uint8_t *V, uint16_t I, uint16_t sp, uint16_t *stack, uint8_t *memory, uint8_t delay_timer, uint8_t sound_timer) {
    wmove(debug_win, 0, 0);

    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    wprintw(debug_win, "Opcode: %04X\n", opcode);
    wprintw(debug_win, "PC:     %04X\n", pc);
    wprintw(debug_win, "I:      %04X\n", I);
    wprintw(debug_win, "SP:     %04X\n", sp);

    wprintw(debug_win, "\nRegisters:\n    ");
    for (int i = 0; i < 16; i++) {
        wprintw(debug_win, "V%X ", i);
    }
    wprintw(debug_win, "\n    ");
    for (int i = 0; i < 16; i++) {
        wprintw(debug_win, "%02X ", V[i]);
    }

    wprintw(debug_win, "\n\nStack: \n    ");
    for (int i = 0; i < 16; i++) {
        wprintw(debug_win, "%04X ", stack[i]);
    }

    wprintw(debug_win, "\nMemory (+I) [%04X-%04X]:\n    ", I, I + 16);
    for (int i = 0; i < 16; i++) {
        wprintw(debug_win, "%02X ", memory[i + I]);
    }
    wprintw(debug_win, "...\n");

    wprintw(debug_win, "\nDelay Timer: %02X\n", delay_timer);
    wprintw(debug_win, "Sound Timer: %02X\n", sound_timer);

    wrefresh(debug_win);
}

#endif // CHIP8_DISPLAY_H
