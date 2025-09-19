#include <ncurses.h>
#include <stddef.h>
#include <string.h>
#include "menu.h"

static WINDOW *menu_create(int optc) {
    WINDOW *menu_win;
    int startx, starty;

    startx = (COLS - 52) / 2;
    starty = (LINES - optc - 4) / 2;

    menu_win = newwin(optc + 5, 52, starty, startx);

    return menu_win;
}

static WINDOW *menu_reset(WINDOW *menu_win, const char *menu_title, int optc, struct menu_option *optv) {
    int i;

    box(menu_win, 0, 0);

    mvwprintw(menu_win, 1, (52 - strlen(menu_title)) / 2, "%s", menu_title);

    for (i = 0; i < optc; i++) {
        mvwprintw(menu_win, 3 + i, 2, "%-48s", optv[i].label);
    }
    wrefresh(menu_win);

    return menu_win;
}

static void menu_select(WINDOW *window, struct menu_option *optv, size_t idx) {
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, 3 + idx, 2, "%-48s", optv[idx].label);
    wattroff(window, COLOR_PAIR(1));

    wrefresh(window);
}

int menu_open(const char *menu_title, size_t optc, struct menu_option *optv) {
    WINDOW *menu_win;
    int ch;
    int current_sel = 0;

    curs_set(0);

    menu_win = menu_create(optc);
    menu_reset(menu_win, menu_title, optc, optv);
    menu_select(menu_win, optv, current_sel);

    while ((ch = getch()) != '\n') {
        switch (ch) {
            case KEY_UP:
                current_sel = current_sel > 0 ? (current_sel - 1) % optc : optc - 1;
                menu_reset(menu_win, menu_title, optc, optv);
                menu_select(menu_win, optv, current_sel);
                break;
            case KEY_DOWN:
                current_sel = (current_sel + 1) % optc;
                menu_reset(menu_win, menu_title, optc, optv);
                menu_select(menu_win, optv, current_sel);
                break;
        }
    }

    delwin(menu_win);

    curs_set(1);

    return current_sel;
}
