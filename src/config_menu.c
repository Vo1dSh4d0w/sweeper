#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_menu.h"
#include "color.h"
#include "config.h"
#include "input_handler.h"
#include "macros.h"
#include "status_bar.h"
#include "win_helpers.h"

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

    // create a new window in the center of the screen
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
    // flip bg and fg colors if the control is selected
    if (select) {
        wattron(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));
    }

    // handle special controls (buttons)
    if (control == cfg->count) {
        curs_set(0);
        mvwprintw(menu_win, cfg->count + 4, 22, "<OK>");
    } else if (control == cfg->count + 1) {
        curs_set(0);
        mvwprintw(menu_win, cfg->count + 4, 68, "<Cancel>");
    } else {
        curs_set(1);
        // print the value if control is not a button
        config_menu_print_config(menu_win, control + 3, 50, cfg->def[control].type, &cfg->val[control].value);
    }
    wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_INVERSE));

    wrefresh(menu_win);
}

/**
 * config_menu_read_input : reads input from a control; this function will not change the window in any way
 * @param window : the window to read input from
 * @param index  : the index of the control to read from
 * @param offset : the count of characters to skip from reading
 * @param out    : the pointer to the char array to write the input to, this will be null-terminated
 * @returns      : the count of characters read
 */
static size_t config_menu_read_input(WINDOW *window, const int index, const size_t offset, char *out) {
    int y, x;
    chtype input[45];
    size_t length;

    // store the current position of the cursor so it can be restored later
    getyx(window, y, x);

    // get the raw value from the screen
    mvwinchnstr(window, index + 3, 50 + offset, input, 44);
    for (length = 0; length < 44; length++) {
        // a space terminates the string, this should be made more robust when string configs are supported
        if ((input[length] & A_CHARTEXT) == ' ') {
            if (out != NULL) {
                // null-terminate the output string
                out[length] = '\0';
            }
            break;
        }
        if (out != NULL) {
            // mask the char
            out[length] = input[length] & A_CHARTEXT;
        }
    }

    wmove(window, y, x);

    return length;
}

/**
 * config_menu_handle_backspace : handle a user input of KEY_BACKSPACE (0x107);
 *                                this function will delete the character left to the cursor,
 *                                shift the rest of the input to the left and move the cursor to the left
 * @param window : the window to handle backspace on
 * @returns      : 1 if a character was deleted, 0 if no character was deleted
 */
static int config_menu_handle_backspace(WINDOW *window) {
    int y, x;
    char input[44];
    size_t length;

    // store the cursor position so we can move it to the left later
    getyx(window, y, x);

    // if the cursor is on the left edge, ignore
    if (x <= 50) {
        return 0;
    }

    // read the input starting from the left edge of the cursor
    length = config_menu_read_input(window, y - 3, x - 50, input);

    if (length - x == 0) {
        return 0;
    }

    // delete the last character to prevent "ghost characters"
    input[length] = ' ';
    input[length + 1] = '\0';

    // shift the input to the left
    mvwprintw(window, y, x - 1, "%s", input);
    // move the cursor to the left
    wmove(window, y, x - 1);

    return 1;
}

/**
 * config_menu_handle_del : handle a user input of KEY_DC (0x14a);
 *                          this function will delete the character right to the cursor,
 *                          shift the rest of the input to the left and keep cursor position
 * @param window : the window to handle del on
 */
static void config_menu_handle_del(WINDOW *window) {
    int y, x;
    char input[44];
    size_t length;

    // store the cursor position so we can restore it later
    getyx(window, y, x);

    // read the input starting from the right edge of the cursor
    length = config_menu_read_input(window, y - 3, x - 49, input);

    // delete the last character to prevent "ghost characters"
    input[length] = ' ';
    input[length + 1] = '\0';

    // shift the input to the left
    mvwprintw(window, y, x, "%s", input);
    // restore cursor position
    wmove(window, y, x);
}

/**
 * config_menu_shift_input : shift input right to the cursor one to the right
 *                           this function is used to make space for a new character
 * @param window : the window to shift input on
 */
static void config_menu_shift_input(WINDOW *window) {
    int y, x;
    char input[44];

    getyx(window, y, x);

    // read input starting from the left edge of the cursor, if there is none, ignore
    if (config_menu_read_input(window, y - 3, x - 50, input)) {
        // shift it to the right
        mvwprintw(window, y, x + 1, "%s", input);
    }

    // restore cursor position
    wmove(window, y, x);
}

/**
 * config_menu_update_input : this function updates the internal storage of the input;
 *                            this will handle overflow, but won't affect the visual state of the window
 * @param window   : the window to read the input from
 * @param cfg_type : the type of the config the cursor is currently on
 * @param value    : the pointer to the value to update
 */
static void config_menu_update_input(WINDOW *window, enum config_type cfg_type, union config_variant *value) {
    int y;
    char input[44];

    y = getcury(window);

    config_menu_read_input(window, y - 3, 0, input);
    // handle different config types differently
    if (cfg_type == CFG_TYPE_NUMBER) {
        // parse the input as a long long, this will make sure the input is in bounds
        value->number = strtoll(input, NULL, 10);
    }
}

/**
 * config_menu_validate_spec : validate the spec attached to a config def on a config value
 * @param index : the index of the config to validate
 * @param cfg : the configuration information
 */
static void config_menu_validate_spec(const int index, struct config_info *cfg) {
    const union config_variant *value;
    union config_variant new_value;
    char status_bar_msg[128];

    value = config_get(cfg->count, cfg->val, cfg->def[index].id);
    memcpy(&new_value, value, sizeof(union config_variant));

    switch (cfg->def[index].type) {
        case CFG_TYPE_NUMBER:
        new_value.number = min(max(value->number, cfg->def[index].spec.number.min), cfg->def[index].spec.number.max);
        if (new_value.number != value->number) {
            sprintf(status_bar_msg, "Invalid value for %s was updated. Changed from %lld to %lld.", cfg->def[index].label, value->number, new_value.number);
            status_bar_message(status_bar_msg);
        }
        break;
        case CFG_TYPE_DECIMAL:
        break;
        case CFG_TYPE_FLAGS:
        break;
    }
    config_set(cfg->count, cfg->val, cfg->def[index].id, new_value);
}

/**
 * config_menu_move_cursor : move the cursor inside a control by a given offset;
 *                           this function will ensure the cursor ends in a valid position
 * @param window : the window to move the cursor on
 * @param index  : the index of the control currently selected
 * @param offset : the offset to move the cursor by
 */
static void config_menu_move_cursor(WINDOW *window, const int index, const int offset) {
    int x, y;
    size_t len;

    getyx(window, y, x);
    len = config_menu_read_input(window, index, 0, NULL);

    if (offset < 0) {
        // ensure the cursor ends in a valid position
        wmove(window, y, max(50, x + offset));
    } else {
        // ensure the cursor ends in a valid position
        wmove(window, y, min(50 + len, x + offset));
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
    char input[44], y, x;
    union config_variant new_value;

    // ignore input if the selected control is a button
    if (sel >= cfg->count) {
        return;
    }

    getyx(window, y, x);
    config_menu_read_input(window, sel, 0, input);

    // handle deleting
    if (isbackspace(ch)) {
        config_menu_handle_backspace(window);
    } else if (ch == KEY_DC) {
        config_menu_handle_del(window);
    } else {
        // if the character is printable, shift the input to the right of the cursor
        // one to the right to make space for the new character
        config_menu_shift_input(window);
        waddch(window, ch);
    }

    // update new_value while validating the input
    config_menu_update_input(window, cfg->def[sel].type, &new_value);
    // set the config to the new value
    config_set(cfg->count, cfg->val, cfg->def[sel].id, new_value);
    // re-print the value in the case validation changed the value (e.g. because of overflow)
    config_menu_print_control(window, sel, 1, cfg);

    wmove(window, y, x);

    // set cursor position
    if (isbackspace(ch)) {
        config_menu_move_cursor(window, sel, -1);
    } else if (ch != KEY_DC) {
        config_menu_move_cursor(window, sel, 1);
    }

    wrefresh(window);
}

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config) {
    WINDOW *menu_win;
    int ch, current_sel = 0;
    struct config_val edited[count];
    // pack all config info into one struct
    struct config_info cfg = {
        .count = count,
        .def = def,
        .val = edited
    };

    // make a working copy of the configuration
    memcpy(edited, config, sizeof(struct config_val) * count);

    // ensure the terminal is always large enough
reset:
    require_terminal_size(count + 6, 96);

    // initialize window
    menu_win = config_menu_create(count);
    config_menu_reset(menu_win, menu_title, &cfg);
    // select the first control
    config_menu_print_control(menu_win, current_sel, 1, &cfg);

    // cancel if user presses ESC
    while ((ch = wgetch(menu_win)) != '\x1b') {
        // handle Ctrl+C / Ctrl+D
        handle_kb_interrupt(ch);

        switch (ch) {
        case KEY_RESIZE:
            clear();
            refresh();
            delwin(menu_win);
            goto reset;
        case KEY_UP:
            if (current_sel < count) {
                config_menu_validate_spec(current_sel, &cfg);
            }
            // change selection
            config_menu_print_control(menu_win, current_sel, 0, &cfg);
            current_sel = current_sel > 0 ? (current_sel - 1): count + 1;
            config_menu_print_control(menu_win, current_sel, 1, &cfg);
            break;
        case KEY_DOWN:
            if (current_sel < count) {
                config_menu_validate_spec(current_sel, &cfg);
            }
            // change selection
            config_menu_print_control(menu_win, current_sel, 0, &cfg);
            current_sel = (current_sel + 1) % (count + 2);
            config_menu_print_control(menu_win, current_sel, 1, &cfg);
            break;
        case KEY_LEFT:
            // change selection if selected control is a button
            if (current_sel == count) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel++;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else if (current_sel == count + 1) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel--;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else {
                // if selected control is an input, move the cursor to the left
                config_menu_move_cursor(menu_win, current_sel, -1);
            }
            break;
        case KEY_RIGHT:
            // change selection if selected control is a button
            if (current_sel == count) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel++;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else if (current_sel == count + 1) {
                config_menu_print_control(menu_win, current_sel, 0, &cfg);
                current_sel--;
                config_menu_print_control(menu_win, current_sel, 1, &cfg);
            } else {
                // if selected control is an input, move the cursor to the right
                config_menu_move_cursor(menu_win, current_sel, 1);
            }
            break;
        case '\n':
            if (current_sel == count) {
                // if user pressed <OK>, make the working copy the actual value
                config_merge(count, count, def, config, edited);
                status_bar_message("Config saved.");
                goto exit;
            }
            if (current_sel == count + 1) {
                // if the user pressed <Cancel>, don't make the working copy the actual value
                status_bar_message("Changes discarded.");
                goto exit;
            }
            break;
        default:
            // if the characer is printable or backspace or DEL, handle it
            if (isprint(ch)) {
        case KEY_BACKSPACE:
        case 0x7f:
        case '\b':
        case KEY_DC:
                config_menu_input(menu_win, current_sel, ch, &cfg);
            }
            break;
        }
    }

exit:
    // clean up
    curs_set(1);
    delwin(menu_win);
}
