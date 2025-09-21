#include "config.h"
#include <stdlib.h>
#include <string.h>

const union config_variant *config_get(const size_t count, const struct config_val *config, const char *id) {
    int i;

    for (i = 0; i < count; i++) {
        if (strcmp(config[i].id, id) == 0) return &config[i].value;
    }

    return NULL;
}

void config_set(const size_t count, struct config_val *config, const char *id, union config_variant value) {
    int i;

    for (i = 0; i < count; i++) {
        if (strcmp(config[i].id, id) == 0) {
            config[i].value = value;
            break;
        }
    }
}

void config_merge(size_t count_into, size_t count_from, const struct config_def *def, struct config_val *into, const struct config_val *from) {
    int i;
    struct config_val copy[count_into];
    const union config_variant *to_copy;

    memcpy(copy, into, sizeof(struct config_val) * count_into);

    for (i = 0; i < count_into; i++) {
        to_copy = config_get(count_from, from, def[i].id);
        if (to_copy == NULL) to_copy = config_get(count_into, copy, def[i].id);
        memcpy(into[i].id, def[i].id, sizeof(char) * 16);
        memcpy(&into[i].value, to_copy, sizeof(union config_variant));
    }
}
