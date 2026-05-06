#include "commands.h"
#include "report.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void cmd_add(const char *district, const char *user, const char *role) {
    setup_district(district);
    
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    if (!check_access(role, path, 0664)) {
        printf("Eroare de permisiuni: %s nu are acces.\n", role);
        return;
    }

    Report r;
    memset(&r, 0, sizeof(Report));
    
    // Setam datele automate (ID random, utilizator, timp)
    r.report_id = rand() % 10000;
    strncpy(r.inspector, user, MAX_STR - 1);
    r.timestamp = time(NULL);

    // --- Citire de la tastatura a datelor raportului ---
    printf("X: ");
    if (scanf("%f", &r.latitude) != 1) r.latitude = 0.0;

    printf("Y: ");
    if (scanf("%f", &r.longitude) != 1) r.longitude = 0.0;


    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Category (road/lighting/flooding/other): ");
    if (fgets(r.category, sizeof(r.category), stdin)) {
        r.category[strcspn(r.category, "\n")] = 0; 
    }

    printf("Severity level (1/2/3): ");
    if (scanf("%d", &r.severity) != 1) r.severity = 1;

    while ((c = getchar()) != '\n' && c != EOF);

    printf("Description: ");
    if (fgets(r.description, sizeof(r.description), stdin)) {
        r.description[strcspn(r.description, "\n")] = 0;
    }

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("Eroare la deschidere reports.dat");
        return;
    }
    write(fd, &r, sizeof(Report));
    close(fd);
    
    log_action(district, user, role, "add");
}

void cmd_list(const char *district, const char *role) {
    (void)role;
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    struct stat file_stat;
    if (stat(path, &file_stat) < 0) return;

    char perm_str[10];
    mode_to_string(file_stat.st_mode, perm_str);
    printf("Fisier: %s | Permisiuni: %s | Bytes: %ld\n\n", path, perm_str, file_stat.st_size);

    int fd = open(path, O_RDONLY);
    if (fd < 0) return;

    Report r;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("ID: %d | Inspector: %s | Cat: %s | Sev: %d\n", 
               r.report_id, r.inspector, r.category, r.severity);
    }
    close(fd);
}

void cmd_remove(const char *district,const char *user, const char *role, int target_id) {
    if (strcmp(role, "manager") != 0) {
        printf("Eroare: Doar managerii pot sterge rapoarte.\n");
        return;
    }

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    int fd = open(path, O_RDWR);
    if (fd < 0) return;

    Report r, next_r;
    int found = 0;
    off_t pos = 0;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.report_id == target_id) { found = 1; break; }
        pos += sizeof(Report);
    }

    if (!found) {
        printf("ID negasit.\n");
        close(fd);
        return;
    }

    off_t read_pos = pos + sizeof(Report);
    off_t write_pos = pos;

    while (1) {
        lseek(fd, read_pos, SEEK_SET);
        if (read(fd, &next_r, sizeof(Report)) <= 0) break;
        lseek(fd, write_pos, SEEK_SET);
        write(fd, &next_r, sizeof(Report));
        read_pos += sizeof(Report);
        write_pos += sizeof(Report);
    }

    ftruncate(fd, write_pos);
    close(fd);
    log_action(district, user, role, "remove_report");
    printf("Raport sters.\n");
    
}