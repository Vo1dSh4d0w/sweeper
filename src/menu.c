#include <ncurses.h>
#include <stddef.h>
#include <string.h>
#include "menu.h"
#include "color.h"
#include "input_handler.h"
#include "win_helpers.h"

static WINDOW *menu_create(int optc) {
    WINDOW *menu_win;
    int startx, starty;

    startx = (COLS - 52) / 2;
    starty = (LINES - optc - 4) / 2;

    menu_win = newwin(optc + 5, 52, starty, startx);
    keypad(menu_win, 1);

    return menu_win;
}

static WINDOW *menu_reset(WINDOW *menu_win, const char *menu_title, int optc, const struct menu_option *optv) {
    int i;

    box(menu_win, 0, 0);

    mvwprintw(menu_win, 1, (52 - strlen(menu_title)) / 2, "%s", menu_title);

    for (i = 0; i < optc; i++) {
        mvwprintw(menu_win, 3 + i, 2, "%-48s", optv[i].label);
    }
    wrefresh(menu_win);

    return menu_win;
}

static void menu_select(WINDOW *window, const struct menu_option *optv, size_t idx) {
    wattron(window, COLOR_PAIR(COLOR_PAIR_INVERSE));
    mvwprintw(window, 3 + idx, 2, "%-48s", optv[idx].label);
    wattroff(window, COLOR_PAIR(COLOR_PAIR_INVERSE));

    wrefresh(window);
}

int menu_open(const char *menu_title, size_t optc, const struct menu_option *optv) {
    WINDOW *menu_win;
    int ch, current_sel = 0;

reset:
    require_terminal_size(optc + 5, 52);
    curs_set(0);

    menu_win = menu_create(optc);
    menu_reset(menu_win, menu_title, optc, optv);
    menu_select(menu_win, optv, current_sel);

    while ((ch = wgetch(menu_win)) != '\n') {
        handle_kb_interrupt(ch);

        switch (ch) {
        case KEY_RESIZE:
            clear();
            refresh();
            delwin(menu_win);
            goto reset;
        case KEY_UP:
            current_sel = current_sel > 0 ? (current_sel - 1): optc - 1;
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
