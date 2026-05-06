#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Utilizare: %s --role <role> --user <name> --<command> <district_id> [args...]\n", argv[0]);
        return 1;
    }

    char *role = NULL, *user = NULL, *command = NULL, *district = NULL;
    int target_id = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--role") == 0) role = argv[++i];
        else if (strcmp(argv[i], "--user") == 0) user = argv[++i];
        else if (strcmp(argv[i], "--add") == 0) { command = "add"; district = argv[++i]; }
        else if (strcmp(argv[i], "--list") == 0) { command = "list"; district = argv[++i]; }
        else if (strcmp(argv[i], "--remove_report") == 0) {
            command = "remove_report";
            district = argv[++i];
            target_id = atoi(argv[++i]);
        }
    }

    if (!role || !user || !command || !district) return 1;

    if (strcmp(command, "add") == 0) cmd_add(district, user, role);
    else if (strcmp(command, "list") == 0) cmd_list(district, role);
    else if (strcmp(command, "remove_report") == 0) cmd_remove(district, user, role, target_id);

    return 0;
}