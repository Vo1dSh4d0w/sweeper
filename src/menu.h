#ifndef MENU_H
#define MENU_H

#include <stddef.h>

struct menu_option {
    int value;
    char label[48];
};

int menu_open(const char *menu_title, size_t optc, const struct menu_option* optv);

#endif
