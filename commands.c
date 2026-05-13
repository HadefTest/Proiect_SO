#include "commands.h"
#include "report.h"
#include "filter.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

void do_add(const char *district, const char *user, const char *role) {
    char dir_path[256], file_path[512], link_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s", district);
    snprintf(file_path, sizeof(file_path), "%s/reports.dat", district);
    snprintf(link_path, sizeof(link_path), "active_reports-%s", district);

    mkdir(dir_path, 0750);
    chmod(dir_path, 0750);

    Report r;
    memset(&r, 0, sizeof(Report));
    
    struct stat st;
    if (stat(file_path, &st) == 0) {
        r.report_id = st.st_size / sizeof(Report);
    } else {
        r.report_id = 0;
    }

    printf("X: "); scanf("%lf", &r.latitude);
    printf("Y: "); scanf("%lf", &r.longitude);
    printf("Category (road/lighting/flooding/other): "); scanf("%s", r.issue_category);
    printf("Severity level (1/2/3): "); scanf("%d", &r.severity_level);
    printf("Description: ");
    char c; while ((c = getchar()) != '\n' && c != EOF); 
    fgets(r.description, sizeof(r.description), stdin);
    r.description[strcspn(r.description, "\n")] = 0;

    strncpy(r.inspector_name, user, sizeof(r.inspector_name) - 1);
    r.timestamp = time(NULL);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd < 0) { perror("Open reports.dat failed"); return; }
    chmod(file_path, 0664);

    write(fd, &r, sizeof(Report));
    close(fd);

    struct stat lst;
    if (lstat(link_path, &lst) < 0) {
        symlink(file_path, link_path);
    }

    int notified = notify_monitor();
    write_log(district, user, role, "add", notified);
}

void do_list(const char *district) {
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/reports.dat", district);

    struct stat st;
    if (stat(file_path, &st) < 0) {
        printf("No reports found or district invalid.\n");
        return;
    }

    char perms[10];
    mode_to_symbolic(st.st_mode, perms);
    printf("File details: %s %ld bytes Last Modified: %ld\n", perms, st.st_size, st.st_mtime);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) return;

    Report r;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("[%d] Category: %s | Severity: %d | Inspector: %s\n", 
               r.report_id, r.issue_category, r.severity_level, r.inspector_name);
    }
    close(fd);
}

void do_view(const char *district, int id) {
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/reports.dat", district);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) return;

    Report r;
    int found = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.report_id == id) {
            printf("ID: %d\nInspector: %s\nCoordinates: %f, %f\nCategory: %s\nSeverity: %d\nTimestamp: %ld\nDescription: %s\n",
                   r.report_id, r.inspector_name, r.latitude, r.longitude, r.issue_category, r.severity_level, r.timestamp, r.description);
            found = 1;
            break;
        }
    }
    close(fd);
    if (!found) printf("Report with ID %d not found.\n", id);
}

void do_remove_report(const char *district, int id, const char *user, const char *role) {
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Permission denied: Only managers can remove reports.\n");
        return;
    }

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/reports.dat", district);

    int fd = open(file_path, O_RDWR);
    if (fd < 0) return;

    struct stat st;
    stat(file_path, &st);
    int total_records = st.st_size / sizeof(Report);

    Report r;
    int found_idx = -1;
    for (int i = 0; i < total_records; i++) {
        lseek(fd, i * sizeof(Report), SEEK_SET);
        read(fd, &r, sizeof(Report));
        if (r.report_id == id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        for (int i = found_idx; i < total_records - 1; i++) {
            lseek(fd, (i + 1) * sizeof(Report), SEEK_SET);
            read(fd, &r, sizeof(Report));
            lseek(fd, i * sizeof(Report), SEEK_SET);
            write(fd, &r, sizeof(Report));
        }
        ftruncate(fd, (total_records - 1) * sizeof(Report));
        printf("Report %d removed successfully.\n", id);
        write_log(district, user, role, "remove_report", 0);
    } else {
        printf("Report %d not found.\n", id);
    }
    close(fd);
}

void do_update_threshold(const char *district, const char *val, const char *user, const char *role) {
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Permission denied.\n");
        return;
    }
    char cfg_path[512];
    snprintf(cfg_path, sizeof(cfg_path), "%s/district.cfg", district);

    if (!check_permission(cfg_path, S_IRUSR) || !check_permission(cfg_path, S_IWUSR)) {
        fprintf(stderr, "Security Alert: district.cfg permissions modified from 640. Aborting operation.\n");
        return;
    }

    int fd = open(cfg_path, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd < 0) return;
    chmod(cfg_path, 0640);

    write(fd, val, strlen(val));
    write(fd, "\n", 1);
    close(fd);
    
    write_log(district, user, role, "update_threshold", 0);
}

void do_filter(const char *district, int cond_count, char **cond_args) {
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/reports.dat", district);

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) return;

    Report r;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        int all_match = 1;
        for (int i = 0; i < cond_count; i++) {
            char field[64], op[16], val[128];
            if (parse_condition(cond_args[i], field, op, val)) {
                if (!match_condition(&r, field, op, val)) {
                    all_match = 0;
                    break;
                }
            } else {
                all_match = 0;
                break;
            }
        }
        if (all_match) {
            printf("[%d] Cat: %s | Sev: %d | Insp: %s | Time: %ld | Desc: %s\n", 
                   r.report_id, r.issue_category, r.severity_level, r.inspector_name, r.timestamp, r.description);
        }
    }
    close(fd);
}

void do_remove_district(const char *district, const char *role) {
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Permission denied. Only managers can destroy a district.\n");
        return;
    }

    char link_path[512];
    snprintf(link_path, sizeof(link_path), "active_reports-%s", district);
    unlink(link_path); 

    pid_t pid = fork();
    if (pid == 0) {
        char dir_path[512];
        snprintf(dir_path, sizeof(dir_path), "./%s", district);
        execlp("rm", "rm", "-rf", dir_path, NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
        printf("District '%s' and related components wiped out.\n", district);
    }
}