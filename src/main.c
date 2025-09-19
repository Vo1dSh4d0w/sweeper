#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include "color.h"
#include "menu.h"
#include "main_menu.h"

int main() {
    initscr();
    noecho();
    raw();
    keypad(stdscr, 1);
    color_init();
    refresh();

    enum main_menu_opts selection = menu_open("Main Menu", 4, main_menu);
    clear();
    switch (selection) {
        case MM_START:
        case MM_STATS:
        case MM_SETTINGS:
            printw("Selected %d", selection);
            refresh();
            break;
        case MM_QUIT:
            endwin();
            return 0;
    }

    getch();
    endwin();

    return EXIT_SUCCESS;
}
