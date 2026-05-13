#include "utils.h"
#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

void mode_to_string(mode_t mode, char *str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}

int check_access(const char *role, const char *path, mode_t expected_mode) {
    struct stat st;
    if (stat(path, &st) < 0) return 0;
    if (strcmp(role, "manager") == 0) {
        return (st.st_mode & expected_mode) == expected_mode;
    }
    // For other roles, maybe different permissions, but for now assume ok
    return 1;
}

void setup_district(const char *district) {
    mkdir(district, 0755);
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0664);
    if (fd >= 0) close(fd);
}

void log_action(const char *district, const char *user, const char *role, const char *action) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/logged_district", district);
    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) return;
    time_t now = time(NULL);
    char buf[256];
    int len = snprintf(buf, sizeof(buf), "%s %s %s %s %ld\n", user, role, action, district, now);
    write(fd, buf, len);
    close(fd);
}