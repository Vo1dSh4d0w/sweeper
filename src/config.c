#include "config.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *config_serialize(const size_t count, const struct config_def *def, const struct config_val *config) {
    char *serialized, buf[48];
    size_t i;

    serialized = malloc(sizeof(char) * count * 48);

    for (i = 0; i < count; i++) {
        strcat(serialized, def[i].id);
        strcat(serialized, "=");
        switch (def[i].type) {
        case CFG_TYPE_NUMBER:
            sprintf(buf, "%lld", config_get(count, config, def[i].id)->number);
            break;
        case CFG_TYPE_DECIMAL:
            sprintf(buf, "%f", config_get(count, config, def[i].id)->decimal);
            break;
        case CFG_TYPE_FLAGS:
            break;
        }
        strcat(serialized, buf);
        strcat(serialized, "\n");
    }
    strcat(serialized, "\0");

    return serialized;
}

size_t config_deserialize(const size_t count, const struct config_def *def, struct config_val *config, const char *serialized) {
    size_t i, j, real_count = 0;
    int n;
    char buffer[48];

    for (i = 0; i < count; i++) {
        if (!sscanf(serialized, "%s%n", buffer, &n)) {
            break;
        }
        serialized += n;

        for (j = 0; j < count; j++) {
            if (strcmp(def[j].id, buffer)) {
                break;
            }
        }

        if (j == count) {
            sscanf(serialized, "%*[^\n]%n", &n);
            serialized += n;
            continue;
        }

        strcpy(buffer, config[real_count].id);

        switch (def[j].type) {
        case CFG_TYPE_NUMBER:
            sscanf(serialized, "=%lld%n", &config[real_count].value.number, &n);
            break;
        case CFG_TYPE_DECIMAL:
            break;
        case CFG_TYPE_FLAGS:
            break;
        }

        serialized += n;
        real_count++;
    }

    return real_count;
}

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
