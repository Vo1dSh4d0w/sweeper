#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "menu.h"

enum main_menu_opts {
    MM_START,
    MM_STATS,
    MM_SETTINGS,
    MM_QUIT,
};

const struct menu_option main_menu[] = {
    {MM_START, "Start"},
    {MM_STATS, "Statistics"},
    {MM_SETTINGS, "Settings"},
    {MM_QUIT, "Quit"}
};

#endif
