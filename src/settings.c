#include "settings.h"
#include "config.h"

const struct config_def settings_def[SETTINGS_LENGTH] = {
    {
        .type = CFG_TYPE_NUMBER,
        .id = SETTINGS_WIDTH,
        .label = "Width",
        .spec.number = {
            .min = 5,
            .max = 150
        }
    },
    {
        .type = CFG_TYPE_NUMBER,
        .id = SETTINGS_HEIGHT,
        .label = "Height",
        .spec.number = {
            .min = 5,
            .max = 50
        }
    }
};

const struct config_val settings_default[SETTINGS_LENGTH] = {
  {
      .id = SETTINGS_WIDTH,
      .value.number = 10
  },
  {
      .id = SETTINGS_HEIGHT,
      .value.number = 10
  }
};

struct config_val settings[SETTINGS_LENGTH];
