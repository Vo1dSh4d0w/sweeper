/**
 * config_menu : a menu window for the user to configure anything
 */

#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H

#include "config.h"

/**
 * config_menu_open : open the configuration window, this function will return when the user finishes configuring
 *                    this will modify the given configuration in-place
 * @param menu_title : a title displayed on top of the configuration window
 * @param count      : the total count of configuration options
 * @param def        : array of configuration definitions (see config.h)
 * @param config     : the current configuration, this must be initialized with the correct ids (see config.h)
 */
void config_menu_open(const char *menu_title, size_t count, const struct config_def *def, struct config_val *config);

#endif
