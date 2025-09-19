#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include "color.h"
#include "menu.h"
#include "main_menu.h"
#include "config_menu.h"
#include "settings.h"
#include "config.h"

struct config_val settings[2];

int main() {
    enum main_menu_opts selection;

    initscr();
    noecho();
    raw();
    keypad(stdscr, 1);
    color_init();
    set_escdelay(25);
    refresh();

    config_merge(2, 2, settings_def, settings, settings_default);

    do {
        selection = menu_open("Main Menu", 4, main_menu);
        clear();
        refresh();

        switch (selection) {
            case MM_START:
            case MM_STATS:
                printw("Selected %d", selection);
                refresh();
                break;
            case MM_SETTINGS:
                config_menu_open("Settings", 2, settings_def, settings);
                clear();
                refresh();
                break;
            case MM_QUIT:
                endwin();
                return 0;
        }
    } while (selection != MM_START);

    getch();
    endwin();

    return EXIT_SUCCESS;
}
