#include <ncurses.h>
#include <unistd.h>
#include "color.h"
#include "menu.h"

enum main_menu_opts {
    MM_START,
    MM_STATS,
    MM_SETTINGS,
    MM_QUIT,
};

int main() {
    struct menu_option main_menu[] = {
        {MM_START, "Start"},
        {MM_STATS, "Statistics"},
        {MM_SETTINGS, "Settings"},
        {MM_QUIT, "Quit"}
    };

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

    return 0;
}
