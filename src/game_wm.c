#include <ncurses.h>
#include "game_wm.h"
#include "game_main_win.h"
#include "game_status_win.h"
#include "input_handler.h"
#include "macros.h"
#include "status_bar.h"
#include "win_helpers.h"

void game_wm_create(struct game_wm *wm) {
    int ch;

    game_main_win_create_window(&wm->main_win);
    game_status_win_create_window(&wm->status_win);

reset:
    require_terminal_size(max(wm->main_win.req_height, wm->status_win.req_height), wm->main_win.req_width + wm->status_win.req_width);

    wm->main_win.curses_win = newwin(wm->main_win.req_height, wm->main_win.req_width, 0, 0);
    wm->status_win.curses_win = newwin(LINES - status_bar_is_enabled(), COLS - wm->main_win.req_width, 0, wm->main_win.req_width);

    wm->main_win.redraw(wm->main_win.curses_win);
    wm->status_win.redraw(wm->status_win.curses_win);

    while ((ch = wgetch(wm->main_win.curses_win)), 1) {
        handle_kb_interrupt(ch);

        switch (ch) {
            case KEY_RESIZE:
            clear();
            refresh();
            goto reset;
        }
    }
}
