#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

int main(int argc, char **argv) {
    char *role = NULL;
    char *user = NULL;
    int cmd_idx = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--role") == 0 && i + 1 < argc) {
            role = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--user") == 0 && i + 1 < argc) {
            user = argv[i + 1];
            i++;
        } else {
            // Aceasta este comanda (ex: --add sau add)
            cmd_idx = i;
            break;
        }
    }

    if (!role) role = "inspector"; 
    if (!user) user = "unknown";

    if (cmd_idx == -1 || cmd_idx >= argc) {
        fprintf(stderr, "Missing command.\n");
        return 1;
    }

    // Extragem comanda
    char *cmd = argv[cmd_idx];

    // MODIFICARE: Dacă comanda începe cu "--", sărim peste primele 2 caractere
    if (strncmp(cmd, "--", 2) == 0) {
        cmd += 2;
    }

    if (strcmp(cmd, "add") == 0 && cmd_idx + 1 < argc) {
        do_add(argv[cmd_idx + 1], user, role);
    } else if (strcmp(cmd, "list") == 0 && cmd_idx + 1 < argc) {
        do_list(argv[cmd_idx + 1]);
    } else if (strcmp(cmd, "view") == 0 && cmd_idx + 2 < argc) {
        do_view(argv[cmd_idx + 1], atoi(argv[cmd_idx + 2]));
    } else if (strcmp(cmd, "remove_report") == 0 && cmd_idx + 2 < argc) {
        do_remove_report(argv[cmd_idx + 1], atoi(argv[cmd_idx + 2]), user, role);
    } else if (strcmp(cmd, "update_threshold") == 0 && cmd_idx + 2 < argc) {
        do_update_threshold(argv[cmd_idx + 1], argv[cmd_idx + 2], user, role);
    } else if (strcmp(cmd, "remove_district") == 0 && cmd_idx + 1 < argc) {
        do_remove_district(argv[cmd_idx + 1], role);
    } else if (strcmp(cmd, "filter") == 0 && cmd_idx + 1 < argc) {
        do_filter(argv[cmd_idx + 1], argc - (cmd_idx + 2), &argv[cmd_idx + 2]);
    } else {
        fprintf(stderr, "Unknown command or malformed arguments.\n");
        return 1;
    }

    return 0;
}