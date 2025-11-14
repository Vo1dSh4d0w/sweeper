#ifndef GAME_WM_H
#define GAME_WM_H

#include <ncurses.h>

struct game_wm_window_data {
    WINDOW *curses_win;
    int req_height;
    int req_width;
    void(*redraw)(WINDOW*);
    void(*accept_input)(WINDOW*, int);
};

struct game_wm {
    struct game_wm_window_data main_win;
    struct game_wm_window_data status_win;
};

void game_wm_create(struct game_wm *wm);

#endif
