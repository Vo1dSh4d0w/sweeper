#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "config_menu.h"
#include "config.h"

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    WINDOW *menu_win;
    int i;

    menu_win = newwin(count + 6, 96, (LINES - count - 5) / 2, (COLS - 96) / 2);
    curs_set(0);

    box(menu_win, 0, 0);
    mvwprintw(menu_win, 1, (96 - strlen(menu_title)) / 2, "%s", menu_title);

    for (i = 0; i < count; i++) {
        mvwprintw(menu_win, i + 3, 2, "%s", def[i].label);
        switch (def[i].type) {
            case CFG_TYPE_NUMBER:
                mvwprintw(menu_win, i + 3, 50, "%lld", config_get(count, config, def[i].id)->number);
                break;
            case CFG_TYPE_DECIMAL:
                mvwprintw(menu_win, i + 3, 50, "%f", config_get(count, config, def[i].id)->decimal);
                break;
            case CFG_TYPE_STRING:
                mvwprintw(menu_win, i + 3, 50, "%s", config_get(count, config, def[i].id)->string);
                break;
            case CFG_TYPE_FLAGS:
                mvwprintw(menu_win, i + 3, 50, "Not Implemented Yet");
                break;
        }
    }

    mvwprintw(menu_win, count + 4, 22, "<OK>");
    mvwprintw(menu_win, count + 4, 68, "<Cancel>");

    wrefresh(menu_win);
    wgetch(menu_win);

    curs_set(1);
    delwin(menu_win);
}
