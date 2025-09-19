#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "config_menu.h"
#include "color.h"
#include "config.h"
#include "input_handler.h"

static WINDOW *config_menu_create(size_t count) {
    WINDOW *menu_win;

    menu_win = newwin(count + 6, 96, (LINES - count - 5) / 2, (COLS - 96) / 2);
    keypad(menu_win, 1);
    set_escdelay(25);

    return menu_win;
}

static void config_menu_print_config(WINDOW *window, int y, int x, const enum config_type type, const union config_variant *value) {
    char buf[48];

    switch (type) {
        case CFG_TYPE_NUMBER:
            sprintf(buf, "%lld", value->number);
            break;
        case CFG_TYPE_DECIMAL:
            sprintf(buf, "%f", value->decimal);
            break;
        case CFG_TYPE_STRING:
            sprintf(buf, "%s", value->string);
            break;
        case CFG_TYPE_FLAGS:
            sprintf(buf, "%s", "<Not Implemented Yet>");
            break;
    }

    mvwprintw(window, y, x, "%-44s", buf);
    wmove(window, y, 50 + strlen(buf));
}

static void config_menu_reset(WINDOW *menu_win, const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    int i;

    box(menu_win, 0, 0);
    mvwprintw(menu_win, 1, (96 - strlen(menu_title)) / 2, "%s", menu_title);

    for (i = 0; i < count; i++) {
        mvwprintw(menu_win, i + 3, 2, "%s", def[i].label);
        config_menu_print_config(menu_win, i + 3, 50, def[i].type, config_get(count, config, def[i].id));
    }

    mvwprintw(menu_win, count + 4, 22, "<OK>");
    mvwprintw(menu_win, count + 4, 68, "<Cancel>");

    wrefresh(menu_win);
}

static void config_menu_select(WINDOW *menu_win, size_t count, int selection, const struct config_def *def, const struct config_val *value) {
    wattron(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));
    if (selection == count) {
        curs_set(0);
        mvwprintw(menu_win, count + 4, 22, "<OK>");
    } else if (selection == count + 1) {
        curs_set(0);
        mvwprintw(menu_win, count + 4, 68, "<Cancel>");
    } else {
        curs_set(1);
        config_menu_print_config(menu_win, selection + 3, 50, def[selection].type, &value->value);
    }
    wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));

    wrefresh(menu_win);
}

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    WINDOW *menu_win;
    int ch, current_sel = 0;

    menu_win = config_menu_create(count);
    config_menu_reset(menu_win, menu_title, count, def, config);
    config_menu_select(menu_win, count, current_sel, def, config);

    while ((ch = wgetch(menu_win)) != '\x1b') {
        handle_kb_interrupt(ch);

        switch (ch) {
            case KEY_UP:
                current_sel = current_sel > 0 ? (current_sel - 1): count + 1;
                config_menu_reset(menu_win, menu_title, count, def, config);
                config_menu_select(menu_win, count, current_sel, def, config);
                break;
            case KEY_DOWN:
                current_sel = (current_sel + 1) % (count + 2);
                config_menu_reset(menu_win, menu_title, count, def, config);
                config_menu_select(menu_win, count, current_sel, def, config);
                break;
        }
    }

    curs_set(1);
    delwin(menu_win);
}
