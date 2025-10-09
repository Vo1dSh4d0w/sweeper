#include <ncurses.h>
#include <string.h>
#include "status_bar.h"
#include "color.h"

static WINDOW *status_bar;
static int status_bar_enabled = 0;
static char message[1024] = "";

static void status_bar_print(char *msg) {
    status_bar_clear();
    mvwprintw(status_bar, 0, 0, "%s", msg);
    wrefresh(status_bar);
}

void status_bar_enable() {
    if (!status_bar_enabled) {
        status_bar = newwin(1, COLS, LINES - 1, 0);
        wattron(status_bar, COLOR_PAIR(COLOR_PAIR_INVERSE));
        status_bar_clear();
        status_bar_enabled = 1;
    }
}

void status_bar_disable() {
    if (status_bar_enabled) {
        wclear(status_bar);
        delwin(status_bar);
        status_bar_enabled = 0;
    }
}

int status_bar_is_enabled() {
    return status_bar_enabled;
}

void status_bar_clear() {
    int i;

    if (status_bar_enabled) {
        wmove(status_bar, 0, 0);
        for (i = 0; i < COLS; i++) {
            waddch(status_bar, ' ');
        }
        wrefresh(status_bar);
    }
}

void status_bar_message(char *msg) {
    strcpy(message, msg);
    if (status_bar_enabled) {
        status_bar_print(msg);
    }
}

void status_bar_refresh() {
    if (status_bar_enabled) {
        touchwin(status_bar);
        wrefresh(status_bar);
    }
}

void status_bar_handle_resize() {
    if (status_bar_enabled) {
        status_bar_disable();
        status_bar_enable();
        status_bar_print(message);
    }
}
