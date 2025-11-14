#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"

#define SETTINGS_WIDTH "WIDTH"
#define SETTINGS_HEIGHT "HEIGHT"

#define SETTINGS_LENGTH 2

#define SETTINGS_WIDTH_V (config_get(SETTINGS_LENGTH, settings, SETTINGS_WIDTH)->number)
#define SETTINGS_HEIGHT_V (config_get(SETTINGS_LENGTH, settings, SETTINGS_HEIGHT)->number)

extern const struct config_def settings_def[];

extern const struct config_val settings_default[];

extern struct config_val settings[SETTINGS_LENGTH];

#endif
