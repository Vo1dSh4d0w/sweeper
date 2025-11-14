#ifndef STATUS_BAR_H
#define STATUS_BAR_H

enum status_bar_message_position {
    sbmp_left,
    sbmp_right,
};

void status_bar_enable();
void status_bar_disable();
int status_bar_is_enabled();

void status_bar_message(char *msg, enum status_bar_message_position pos);
void status_bar_clear();

void status_bar_refresh();
void status_bar_handle_resize();

#endif
