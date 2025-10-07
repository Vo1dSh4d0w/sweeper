#ifndef FILES_H
#define FILES_H

#include <stddef.h>

#define CONFIG_FILE_READ_CHUNK_SIZE 64

void save_config_file(char *filename, char *content);
char *read_config_file(char *filename);

#endif
