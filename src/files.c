#include "files.h"
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void save_config_file(char *filename, char *content) {
    FILE *file;

    file = fopen(filename, "w");
    if (file == NULL) {
        endwin();
        printf("Error while opening file %s, errno %d", filename, errno);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", content);
    fclose(file);
}

char *read_config_file(char *filename) {
    FILE *file;
    char *buf, ch;
    size_t chunks = 1, len = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            return NULL;
        }
        endwin();
        printf("Error while opening file %s, errno %d", filename, errno);
        exit(EXIT_FAILURE);
    }
    buf = malloc(sizeof(char) * CONFIG_FILE_READ_CHUNK_SIZE * chunks);

    while ((ch = fgetc(file)) != EOF) {
        if (len >= chunks * CONFIG_FILE_READ_CHUNK_SIZE) {
            buf = realloc(buf, sizeof(char) * CONFIG_FILE_READ_CHUNK_SIZE * ++chunks);
        }
        buf[len++] = ch;
    }

    return buf;
}
