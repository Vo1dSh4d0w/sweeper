#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

union config_variant {
    unsigned long long number;
    double decimal;
    int flags;
};

enum config_type {
    CFG_TYPE_NUMBER,
    CFG_TYPE_DECIMAL,
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

char *config_serialize(const size_t count, const struct config_val *config);
size_t *config_deserialize(struct config_val *config, const char *serialized);
const union config_variant *config_get(const size_t count, const struct config_val *config, const char id[16]);
void config_set(const size_t count, struct config_val *config, const char id[16], union config_variant value);
void config_merge(size_t count_into, size_t count_from, const struct config_def *def, struct config_val *into, const struct config_val *from);

#endif
