#include "color.h"
#include <ncurses.h>

void color_init() {
    start_color();
    init_pair(COLOR_PAIR_INVERSE, COLOR_BLACK, COLOR_WHITE);
    use_default_colors();
}
