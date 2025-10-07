#include <ncurses.h>
#include <unistd.h>
#include "win_helpers.h"
#include "input_handler.h"

int require_terminal_size(int height, int width) {
    int ch, resized = 0;

    while (LINES < height || COLS < width) {
        resized = 1;

        clear();
        printw("Terminal size too small, needed: Width = %d Height = %d; Current: Width = %d Height = %d", width, height, COLS, LINES);
        refresh();

        ch = getch();
        handle_kb_interrupt(ch);
    }

    if (resized) {
        clear();
        refresh();
    }

    return resized;
}
