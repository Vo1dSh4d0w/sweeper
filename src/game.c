#include <ncurses.h>
#include "game.h"
#include "game_wm.h"

void game_start() {
    struct game_wm wm;

    game_wm_create(&wm);
}
