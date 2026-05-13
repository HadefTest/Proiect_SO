#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

void mode_to_symbolic(mode_t mode, char *buf);
int check_permission(const char *path, mode_t required_bit);
void write_log(const char *district, const char *user, const char *role, const char *action, int monitor_notified);
int notify_monitor(void);

#endif