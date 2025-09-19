#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include "color.h"
#include "menu.h"
#include "main_menu.h"
#include "config_menu.h"
#include "settings.h"
#include "config.h"

int main() {
    enum main_menu_opts selection;
    struct config_val *config;

    initscr();
    noecho();
    raw();
    keypad(stdscr, 1);
    color_init();
    refresh();

    config = malloc(sizeof(struct config_val) * 2);
    config_merge(2, settings_def, config, settings_default);

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
                config_menu_open("Settings", 2, settings_def, config);
                clear();
                refresh();
                break;
            case MM_QUIT:
                endwin();
                free(config);
                return 0;
        }
    } while (selection != MM_START);

    getch();
    endwin();

    free(config);

    return EXIT_SUCCESS;
}
