#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H

#include "config.h"

void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config);

#endif
