#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"

#define SETTINGS_WIDTH "WIDTH"
#define SETTINGS_HEIGHT "HEIGHT"

const struct config_def settings_def[] = {
    {
        .type = CFG_TYPE_NUMBER,
        .id = SETTINGS_WIDTH,
        .label = "Width",
    },
    {
        .type = CFG_TYPE_NUMBER,
        .id = SETTINGS_HEIGHT,
        .label = "Height"
    }
};

const struct config_val settings_default[] = {
  {
      .id = SETTINGS_WIDTH,
      .value = 10,
  },
  {
      .id = SETTINGS_HEIGHT,
      .value = 10,
  }
};

extern struct config_val settings[2];

#endif
