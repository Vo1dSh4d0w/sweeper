#include "config.h"
#include <stdlib.h>
#include <string.h>

const union config_variant *config_get(const size_t count, const struct config_val *config, const char id[16]) {
    int i;

    // iterate through the configuration array until the value with the given id is found
    for (i = 0; i < count; i++) {
        if (strcmp(config[i].id, id) == 0) return &config[i].value;
    }

    // if we are here, it means that the gived id was not found
    return NULL;
}

void config_set(const size_t count, struct config_val *config, const char id[16], union config_variant value) {
    int i;

    // iterate through the configuration array until the value with the given id is found
    for (i = 0; i < count; i++) {
        if (strcmp(config[i].id, id) == 0) {
            config[i].value = value;
            break;
        }
    }
}

void config_merge(size_t count_into, size_t count_from, const struct config_def *def, struct config_val *into, const struct config_val *from) {
    int i;
    struct config_val copy[count_into]; // unmodified copy of the first configuration array
    const union config_variant *to_copy; // temporary reference to the configuration value currently being copied into the first configuration array

    memcpy(copy, into, sizeof(struct config_val) * count_into);

    // iterate through the definition
    for (i = 0; i < count_into; i++) {
        // first, look if the second configuration array contains the id
        to_copy = config_get(count_from, from, def[i].id);
        // if the second configuration array does not contain the id, we take the value of the first configuration array
        if (to_copy == NULL) to_copy = config_get(count_into, copy, def[i].id);
        // copy the id
        memcpy(into[i].id, def[i].id, sizeof(char) * 16);
        // copy the value
        memcpy(&into[i].value, to_copy, sizeof(union config_variant));
    }
}
