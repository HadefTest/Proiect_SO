#ifndef COMMANDS_H
#define COMMANDS_H

void cmd_add(const char *district, const char *user, const char *role);
void cmd_list(const char *district, const char *role);
void cmd_remove(const char *district, const char *user, const char *role, int target_id);

#endif