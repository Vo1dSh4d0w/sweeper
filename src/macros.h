#ifndef MACROS_H
#define MACROS_H
#include <ncurses.h>

/*
 * discard : tell the compiler that a variable is intentionally unused
 */
#define discard(x) ((void)x)
/*
 * min : get the smaller value out of 2 values
 */
#define min(a, b) ((a) < (b) ? (a) : (b))
/*
 * max : get the larger value out of 2 values
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * isbackspace : robust check whether a key is a backspace
 */
#define isbackspace(c) ((c) == KEY_BACKSPACE || (c) == 0x7f || (c) == '\b')

#endif
