#include <ncurses.h>
#include "game_wm.h"
#include "game_status_win.h"

static void game_status_win_redraw(WINDOW *win) {
    box(win, 0, 0);
    wrefresh(win);
}

void game_status_win_create_window(struct game_wm_window_data *win_data) {
    win_data->req_height = 10;
    win_data->req_width = 30;
    win_data->redraw = game_status_win_redraw;
}
