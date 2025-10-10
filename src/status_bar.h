#ifndef STATUS_BAR_H
#define STATUS_BAR_H

void status_bar_enable();
void status_bar_disable();
int status_bar_is_enabled();

void status_bar_message(char *msg);
void status_bar_clear();

void status_bar_refresh();
void status_bar_handle_resize();

#endif
