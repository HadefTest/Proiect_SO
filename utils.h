#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

void mode_to_string(mode_t mode, char *str);
int check_access(const char *role, const char *path, mode_t expected_mode);
void setup_district(const char *district);
void log_action(const char *district, const char *user, const char *role, const char *action);

#endif