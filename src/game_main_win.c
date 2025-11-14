#include <ncurses.h>
#include "game_wm.h"
#include "game_main_win.h"
#include "settings.h"

static void game_main_win_redraw(WINDOW *win) {
    int x, y;

    box(win, 0, 0);

    for (x = 0; x < SETTINGS_WIDTH_V; x++) {
        for (y = 0; y < SETTINGS_HEIGHT_V; y++) {
            mvwprintw(win, y + 1, x + 1, "x");
        }
    }

    wrefresh(win);
}

void game_main_win_create_window(struct game_wm_window_data *win_data) {
    win_data->req_height = SETTINGS_HEIGHT_V + 2;
    win_data->req_width = SETTINGS_WIDTH_V + 2;
    win_data->redraw = game_main_win_redraw;
}
