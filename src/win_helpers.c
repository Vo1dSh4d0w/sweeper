#include <ncurses.h>
#include <unistd.h>
#include "win_helpers.h"
#include "input_handler.h"
#include "status_bar.h"

int require_terminal_size(int height, int width) {
    int ch, resized = 0, req_height;

    req_height = height + status_bar_is_enabled() * 2;

    while (LINES < req_height || COLS < width) {
        resized = 1;

        clear();
        printw("Terminal size too small, needed: Width = %d Height = %d; Current: Width = %d Height = %d", width, req_height, COLS, LINES);
        refresh();

        ch = getch();
        handle_kb_interrupt(ch);
    }

    if (resized) {
        clear();
        refresh();
    }
    status_bar_handle_resize();

    return resized;
}
