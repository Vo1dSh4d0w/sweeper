#include "files.h"
#include <dirent.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *get_config_root() {
    char *path = calloc(FILENAME_MAX, sizeof(char));

    strcpy(path, getenv("HOME"));
    strcat(path, "/.config/sweeper");

    return path;
}

static void ensure_cfg_root_exists() {
    DIR *dir;
    char *path;

    path = get_config_root();
    dir = opendir(path);

    if (dir) {
        closedir(dir);
    } else if (errno == ENOENT) {
        mkdir(path, 0700);
    } else {
        printf("Error while opening directory %s, errno %d", path, errno);
        exit(EXIT_FAILURE);
    }
    free(path);
}

void save_config_file(char *filename, char *content) {
    FILE *file;
    char *path;

    ensure_cfg_root_exists();
    path = get_config_root();
    strcat(path, "/");
    strcat(path, filename);

    file = fopen(path, "w");
    if (file == NULL) {
        endwin();
        printf("Error while opening file %s, errno %d", path, errno);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", content);
    fclose(file);
    free(path);
}

char *read_config_file(char *filename) {
    FILE *file;
    char *path, *buf, ch;
    size_t chunks = 1, len = 0;

    path = get_config_root();
    strcat(path, "/");
    strcat(path, filename);

    file = fopen(path, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            return NULL;
        }
        endwin();
        printf("Error while opening file %s, errno %d", path, errno);
        exit(EXIT_FAILURE);
    }
    buf = malloc(sizeof(char) * CONFIG_FILE_READ_CHUNK_SIZE * chunks);

    while ((ch = fgetc(file)) != EOF) {
        if (len >= chunks * CONFIG_FILE_READ_CHUNK_SIZE) {
            buf = realloc(buf, sizeof(char) * CONFIG_FILE_READ_CHUNK_SIZE * ++chunks);
        }
        buf[len++] = ch;
    }

    free(path);

    return buf;
}
