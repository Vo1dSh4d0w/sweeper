#include "input_handler.h"
#include <ncurses.h>
#include <stdlib.h>

void handle_kb_interrupt(int ch) {
    if (ch != 0x03 && ch != 0x04) return;
    endwin();
    printf("Keyboard Interrupt\n");
    exit(EXIT_FAILURE);
}
