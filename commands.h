#ifndef COMMANDS_H
#define COMMANDS_H

void do_add(const char *district, const char *user, const char *role);
void do_list(const char *district);
void do_view(const char *district, int id);
void do_remove_report(const char *district, int id, const char *user, const char *role);
void do_update_threshold(const char *district, const char *val, const char *user, const char *role);
void do_filter(const char *district, int cond_count, char **cond_args);
void do_remove_district(const char *district, const char *role);

#endif