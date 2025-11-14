#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include "color.h"
#include "files.h"
#include "game.h"
#include "macros.h"
#include "menu.h"
#include "main_menu.h"
#include "config_menu.h"
#include "settings.h"
#include "config.h"
#include "status_bar.h"

int main() {
    enum main_menu_opts selection;
    char *serialized_cfg, status_bar_msg[64];
    struct config_val settings_from_file[2];
    size_t settings_from_file_count;

    initscr();
    noecho();
    raw();
    keypad(stdscr, 1);
    color_init();
    set_escdelay(25);
    refresh();

    status_bar_enable();
#ifdef SWEEPER_VERSION
    sprintf(status_bar_msg, "Sweeper %s", SWEEPER_VERSION);
    status_bar_message(status_bar_msg, sbmp_right);
#else
    discard(status_bar_msg);
#endif

    config_merge(2, 2, settings_def, settings, settings_default);
    serialized_cfg = read_config_file("sweeper.conf");
    if (serialized_cfg != NULL) {
        settings_from_file_count = config_deserialize(2, settings_def, settings_from_file, serialized_cfg);
        config_merge(2, settings_from_file_count, settings_def, settings, settings_from_file);
        free(serialized_cfg);
    }

    do {
        selection = menu_open("Main Menu", 4, main_menu);
        clear();
        refresh();

        switch (selection) {
        case MM_START:
            game_start();
            break;
        case MM_STATS:
            printw("Selected %d", selection);
            refresh();
            break;
        case MM_SETTINGS:
            config_menu_open("Settings", 2, settings_def, settings);
            serialized_cfg = config_serialize(2, settings_def, settings);
            save_config_file("sweeper.conf", serialized_cfg);
            free(serialized_cfg);

            clear();
            refresh();
            break;
        case MM_QUIT:
            endwin();
            return 0;
        }
    } while (selection != MM_START);

    getch();
    endwin();

    return EXIT_SUCCESS;
}
