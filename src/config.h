#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

union config_variant {
    long long number;
    double decimal;
    char *string;
    int flags;
};

enum config_type {
    CFG_TYPE_NUMBER,
    CFG_TYPE_DECIMAL,
    CFG_TYPE_STRING,
    CFG_TYPE_FLAGS
};

struct config_def {
    enum config_type type;
    char id[16];
    char label[48];
};

struct config_val {
    char id[16];
    union config_variant value;
};

char *config_serialize(size_t count, struct config_val *config);
size_t *config_deserialize(struct config_val *config, char *serialized);
union config_variant *config_get(struct config_val *config, char id[16]);

#endif
