#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_menu.h"
#include "color.h"
#include "config.h"
#include "input_handler.h"

/**
 * struct config_info : an array of configuration definitions, configuration values and its count grouped together
 */
struct config_info {
    size_t count;
    const struct config_def *def;
    struct config_val *val;
};

/**
 * config_menu_create : creates a properly sized, empty ncurses window;
 *                      the window will have keypad enabled
 * @param count : the count of configuration options
 * @returns     : the pointer to the newly created window
 */
static WINDOW *config_menu_create(size_t count) {
    WINDOW *menu_win;

    menu_win = newwin(count + 6, 96, (LINES - count - 5) / 2, (COLS - 96) / 2);
    keypad(menu_win, 1);

    return menu_win;
}

/**
 * config_menu_print_config : display a single configuration value;
 *                            this function won't call refresh on the window
 * @param window : the window to display the configuration value on
 * @param y      : the window-relative row to write the configuration value at
 * @param x      : the window-relative column to write the configuration value at
 * @param type   : the type of the configuration value
 * @param value  : the value of the configuration value
 */
static void config_menu_print_config(WINDOW *window, int y, int x, const enum config_type type, const union config_variant *value) {
    char buf[48]; // store a string representation of the value so we can later determine the length of it

    switch (type) {
        // use correct format and union member based on type
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

    // print the value with a padding to the end of window and move the cursor to the right of the last character
    mvwprintw(window, y, x, "%-44s", buf);
    wmove(window, y, x + strlen(buf));
}

/**
 * config_menu_reset : reprint the whole window;
 *                     this function will call refresh on the window
 * @param menu_win   : the window to reprint
 * @param menu_title : the title of the menu to print on top of the window
 * @param cfg        : the configuration information
 */
static void config_menu_reset(WINDOW *menu_win, const char *menu_title, const struct config_info *cfg) {
    int i;

    // print the box around the window
    box(menu_win, 0, 0);
    // print the window title centered
    mvwprintw(menu_win, 1, (96 - strlen(menu_title)) / 2, "%s", menu_title);

    // print every configuration option's label and value
    for (i = 0; i < cfg->count; i++) {
        mvwprintw(menu_win, i + 3, 2, "%s", cfg->def[i].label);
        config_menu_print_config(menu_win, i + 3, 50, cfg->def[i].type, config_get(cfg->count, cfg->val, cfg->def[i].id));
    }

    // print control buttons
    mvwprintw(menu_win, cfg->count + 4, 22, "<OK>");
    mvwprintw(menu_win, cfg->count + 4, 68, "<Cancel>");

    wrefresh(menu_win);
}

/**
 * config_menu_print_control : print the selected configuration option or control button;
 *                             this function will call refresh on the window
 * @param menu_win  : the window to print the control on
 * @param control   : the index of the selection to print;
 *                    values between 0 and count - 1 will be treated as selections of values;
 *                    a value of count will be treated as selection of <OK> control button
 *                    a value of count + 1 will be treated as selection of <Cancel> control button
 * @param select    : whether or not to highlight the control
 * @param cfg       : the configuration information
 */
static void config_menu_print_control(WINDOW *menu_win, int control, int select, const struct config_info *cfg) {
    if (select) {
        wattron(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));
    }

    if (control == cfg->count) {
        curs_set(0);
        mvwprintw(menu_win, cfg->count + 4, 22, "<OK>");
    } else if (control == cfg->count + 1) {
        curs_set(0);
        mvwprintw(menu_win, cfg->count + 4, 68, "<Cancel>");
    } else {
        curs_set(1);
        config_menu_print_config(menu_win, control + 3, 50, cfg->def[control].type, &cfg->val[control].value);
    }
    wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));

    wrefresh(menu_win);
}

static size_t config_menu_read_input(WINDOW *window, const int index, const size_t offset, char *out) {
    int y, x;
    chtype input[44];
    size_t length;

    getyx(window, y, x);

    mvwinchnstr(window, index + 3, 50 + offset, input, 44);
    for (length = 0; length < 44; length++) {
        if ((input[length] & A_CHARTEXT) == ' ') {
            if (out != NULL) {
                out[length] = '\0';
            }
            break;
        }
        if (out != NULL) {
            out[length] = input[length] & A_CHARTEXT;
        }
    }

    wmove(window, y, x);

    return length;
}

static int config_menu_handle_backspace(WINDOW *window) {
    int y, x;
    char input[44];
    size_t length;

    getyx(window, y, x);

    if (x <= 50) {
        return 0;
    }

    length = config_menu_read_input(window, y - 3, x - 50, input);

    if (length - x == 0) {
        return 0;
    }

    input[length] = ' ';
    input[length + 1] = '\0';

    mvwprintw(window, y, x - 1, "%s", input);
    wmove(window, y, x - 1);

    return 1;
}

static void config_menu_shift_input(WINDOW *window) {
    int y, x;
    char input[44];

    getyx(window, y, x);

    if (config_menu_read_input(window, y - 3, x - 50, input)) {
        mvwprintw(window, y, x + 1, "%s", input);
    }

    wmove(window, y, x);
}

static void config_menu_update_input(WINDOW *window, enum config_type cfg_type, union config_variant *value) {
    int y;
    char input[44];

    y = getcury(window);

    config_menu_read_input(window, y - 3, 0, input);
    if (cfg_type == CFG_TYPE_NUMBER) {
        value->number = strtoll(input, NULL, 10);
    }
}

/**
 * config_menu_input : handle input in the menu;
 *                     this function will modify the configuration value;
 *                     this function will call refresh on the window
 * @param window : the window to handle input on
 * @param sel    : the index of current selection
 * @param ch     : the character to handle
 * @param cfg    : the configuration information
 */
static void config_menu_input(WINDOW *window, const int sel, const int ch, struct config_info *cfg) {
    char input[44];
    union config_variant new_value;

    config_menu_read_input(window, sel, 0, input);

    if (ch == KEY_BACKSPACE) {
        config_menu_handle_backspace(window);
    } else {
        config_menu_shift_input(window);
        waddch(window, ch);
    }

    config_menu_update_input(window, cfg->def[sel].type, &new_value);
    config_set(cfg->count, cfg->val, cfg->def[sel].id, new_value);

    wrefresh(window);
}

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    WINDOW *menu_win;
    int ch, current_sel = 0, y, x;
    struct config_val edited[count];
    struct config_info cfg = {
        .count = count,
        .def = def,
        .val = edited
    };

    memcpy(edited, config, sizeof(struct config_val) * count);

    menu_win = config_menu_create(count);
    config_menu_reset(menu_win, menu_title, &cfg);
    config_menu_print_control(menu_win, current_sel, 1, &cfg);

    while ((ch = wgetch(menu_win)) != '\x1b') {
        handle_kb_interrupt(ch);

        switch (ch) {
        case KEY_UP:
            config_menu_print_control(menu_win, current_sel, 0, &cfg);
            current_sel = current_sel > 0 ? (current_sel - 1): count + 1;
            config_menu_print_control(menu_win, current_sel, 1, &cfg);
            break;
        case KEY_DOWN:
            config_menu_print_control(menu_win, current_sel, 0, &cfg);
            current_sel = (current_sel + 1) % (count + 2);
            config_menu_print_control(menu_win, current_sel, 1, &cfg);
            break;
        case KEY_LEFT:
            if (current_sel == count) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel++;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else if (current_sel == count + 1) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel--;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else {
                getyx(menu_win, y, x);
                if (x > 50) {
                    wmove(menu_win, y, x - 1);
                }
            }
            break;
        case KEY_RIGHT:
            if (current_sel == count) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel++;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else if (current_sel == count + 1) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel--;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
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
                config_menu_input(menu_win, current_sel, ch, &cfg);
                getyx(menu_win, y, x);
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
                wmove(menu_win, y, x);
            }
            break;
        }
    }

exit:
    curs_set(1);
    delwin(menu_win);
}
