#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_menu.h"
#include "color.h"
#include "config.h"
#include "input_handler.h"

static WINDOW *config_menu_create(size_t count) {
    WINDOW *menu_win;

    menu_win = newwin(count + 6, 96, (LINES - count - 5) / 2, (COLS - 96) / 2);
    keypad(menu_win, 1);

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
        case CFG_TYPE_FLAGS:
            sprintf(buf, "%s", "<Not Implemented Yet>");
            break;
    }

    mvwprintw(window, y, x, "%-44s", buf);
    wmove(window, y, x + strlen(buf));
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
        config_menu_print_config(menu_win, selection + 3, 50, def[selection].type, &value[selection].value);
    }
    wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));

    wrefresh(menu_win);
}

static void config_menu_input(WINDOW *window, const size_t count, const struct config_def *def, const int sel, const int ch, struct config_val *config) {
    int y, x, i;
    chtype input[44];
    char raw_input[44];
    size_t len;
    union config_variant new_value;

    getyx(window, y, x);
    mvwinchnstr(window, y, 50, input, 44);
    wmove(window, y, x);
    for (len = 0; len < 44; len++) {
        if ((input[len] & A_CHARTEXT) == ' ') break;
    }
    if (ch == KEY_BACKSPACE) {
        mvwinchnstr(window, y, x, input, 44);
        for (i = 0; i < 43; i++) {
            if ((input[i] & A_CHARTEXT) == ' ') {
                raw_input[i] = ' ';
                raw_input[i + 1] = '\0';
                break;
            }
            raw_input[i] = input[i] & A_CHARTEXT;
        }
        mvwprintw(window, y, x - 1, "%s", raw_input);
    } else if (len == x - 50) {
        waddch(window, ch);
    } else {
        mvwinchnstr(window, y, x, input, 44);
        for (len = 0; len < 44; len++) {
            if ((input[len] & A_CHARTEXT) == ' ') {
                raw_input[len] = '\0';
                break;
            }
            raw_input[len] = input[len] & A_CHARTEXT;
        }
        mvwprintw(window, y, x + 1, "%s", raw_input);
        wmove(window, y, x);
        waddch(window, ch);
    }
    mvwinchnstr(window, y, 50, input, 44);
    for (len = 0; len < 44; len++) {
        if ((input[len] & A_CHARTEXT) == ' ') {
            raw_input[len] = '\0';
            break;
        }
        raw_input[len] = input[len] & A_CHARTEXT;
    }
    if (def[sel].type == CFG_TYPE_NUMBER) {
        new_value.number = strtoll(raw_input, NULL, 10);
    }
    config_set(count, config, def[sel].id, new_value);
    wmove(window, y, x + (ch == KEY_BACKSPACE ? -1 : 1));
    wrefresh(window);
}

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    WINDOW *menu_win;
    int ch, current_sel = 0, y, x;
    struct config_val edited[count];

    memcpy(edited, config, sizeof(struct config_val) * count);

    menu_win = config_menu_create(count);
    config_menu_reset(menu_win, menu_title, count, def, edited);
    config_menu_select(menu_win, count, current_sel, def, edited);

    while ((ch = wgetch(menu_win)) != '\x1b') {
        handle_kb_interrupt(ch);

        switch (ch) {
            case KEY_UP:
                current_sel = current_sel > 0 ? (current_sel - 1): count + 1;
                config_menu_reset(menu_win, menu_title, count, def, edited);
                config_menu_select(menu_win, count, current_sel, def, edited);
                break;
            case KEY_DOWN:
                current_sel = (current_sel + 1) % (count + 2);
                config_menu_reset(menu_win, menu_title, count, def, edited);
                config_menu_select(menu_win, count, current_sel, def, edited);
                break;
            case KEY_LEFT:
                if (current_sel == count) {
                    current_sel++;
                    config_menu_reset(menu_win, menu_title, count, def, edited);
                    config_menu_select(menu_win, count, current_sel, def, edited);
                } else if (current_sel == count + 1) {
                    current_sel--;
                    config_menu_reset(menu_win, menu_title, count, def, edited);
                    config_menu_select(menu_win, count, current_sel, def, edited);
                } else {
                    getyx(menu_win, y, x);
                    if (x > 50) {
                        wmove(menu_win, y, x - 1);
                    }
                }
                break;
            case KEY_RIGHT:
                if (current_sel == count) {
                    current_sel++;
                    config_menu_reset(menu_win, menu_title, count, def, edited);
                    config_menu_select(menu_win, count, current_sel, def, edited);
                } else if (current_sel == count + 1) {
                    current_sel--;
                    config_menu_reset(menu_win, menu_title, count, def, edited);
                    config_menu_select(menu_win, count, current_sel, def, edited);
                } else {
                    getyx(menu_win, y, x);
                    if (x < 90) {
                        wmove(menu_win, y, x + 1);
                    }
                }
                break;
            case '\n':
                if (current_sel == count) {
                    config_merge(count, count, def, config, edited);
                    goto exit;
                }
                if (current_sel == count + 1) {
                    goto exit;
                }
                break;
            default:
                if (isprint(ch)) {
            case KEY_BACKSPACE:
            case KEY_DL:
                    config_menu_input(menu_win, count, def, current_sel, ch, edited);
                    getyx(menu_win, y, x);
                    config_menu_select(menu_win, count, current_sel, def, edited);
                    wmove(menu_win, y, x);
                }
                break;
        }
    }

exit:
    curs_set(1);
    delwin(menu_win);
}
