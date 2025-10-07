#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

/**
 * union config_variant : a single configuration value
 *
 * the union member to use should be based on the type set in struct config_def
 */
union config_variant {
    unsigned long long number;      // use if type equals CFG_TYPE_NUMBER
    double decimal;                 // use if type equals CFG_TYPE_DECIMAL
    int flags;                      // use if type equals CFG_TYPE_FLAGS
};

/**
 * enum config_type : determines which union member of config_variant to use
 */
enum config_type {
    CFG_TYPE_NUMBER,                // corresponds to config_variant.number
    CFG_TYPE_DECIMAL,               // corresponds to config_variant.decimal
    CFG_TYPE_FLAGS                  // corresponds to config_variant.flags
};

/**
 * struct config_def : a single definition for a configuration parameter
 */
struct config_def {
    enum config_type type;          // the type of the configuration value
    char id[16];                    // the id of the configuration parameter, is used in serialization/deserialization, must be unique
    char label[48];                 // the user-friendly display label used to describe the configuration parameter
};

/**
 * struct config_val : a single configuration value mapped to an id
 */
struct config_val {
    char id[16];                    // the id matching the id in struct config_def
    union config_variant value;     // the value of this configuration parameter
};

/**
 * config_serialize : serialize an array of configuration values into a char array
 *                    the returned string will have a format of <id>=<value>, lines will be terminated with a single LF
 *                    and the char array will be terminated with a single null character
 *
 * @param count  : the total count of configuration values
 * @param def    : array of configuration definitions
 * @param config : array of configuration values, each config must have a corresponding definition
 *
 * @returns      : pointer to a char array containing the serialized value
 */
char *config_serialize(const size_t count, const struct config_def *def, const struct config_val *config);

/**
 * config_deserialize : deserialize a previously serialized char array of configuration values
 *
 * @param count      : the count of configuration definitions provided
 * @param def        : the definition of the values to be expected in the char array
 * @param config     : the pointer to allocated memory for the configuration values to be written to
 * @param serialized : the pointer to the serialized char array
 *
 * @returns          : the count of the actual configuration values found and deserialized into config
 */
size_t config_deserialize(const size_t count, const struct config_def *def, struct config_val *config, const char *serialized);

/**
 * config_get : get a single configuration value out of an array of configuration values by id
 *
 * @param count  : the count of configuration values in the array
 * @param config : the pointer to the array of configuration values to find the config in
 * @param id     : the id of the configuration value to search for
 *
 * @returns      : the pointer to the found configuration value and NULL if no value corresponding to the id was found
 */
const union config_variant *config_get(const size_t count, const struct config_val *config, const char id[16]);

/**
 * config_set : set a single configuration value in an array of configuration values by id,
 *              if id is not found in the config, this function will do nothing
 *
 * @param count  : the count of configuration values in the array
 * @param config : the pointer to the array of configuration values to set the config in
 * @param id     : the id of the config to set
 * @param value  : the value of the config to set
 */
void config_set(const size_t count, struct config_val *config, const char id[16], union config_variant value);

/**
 * config_merge : merges two arrays of configuration values together;
 *                The second configuration will be merged into the first one overwriting every configuration value with the same id.
 *                No new ids will be introduced in the first configuration array.
 *                This function might reorder the first configuration array.
 *
 * @param count_into : the count of configuration values in the first configuration array
 * @param count_from : the count of configuration values in the second configuration array
 * @param def        : the configuration definition, it should define the configuration values in the first configuration array
 * @param into       : the first configuration array, it will be modified and possibly reordered by this function
 * @param from       : the second configuration array, it will stay untouched by the function
 */
void config_merge(size_t count_into, size_t count_from, const struct config_def *def, struct config_val *into, const struct config_val *from);

#endif
