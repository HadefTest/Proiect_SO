#include "utils.h"
#include "report.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void mode_to_string(mode_t mode, char *str) {
    strcpy(str, "---------");
    if (mode & S_IRUSR) str[0] = 'r';
    if (mode & S_IWUSR) str[1] = 'w';
    if (mode & S_IXUSR) str[2] = 'x';
    if (mode & S_IRGRP) str[3] = 'r';
    if (mode & S_IWGRP) str[4] = 'w';
    if (mode & S_IXGRP) str[5] = 'x';
    if (mode & S_IROTH) str[6] = 'r';
    if (mode & S_IWOTH) str[7] = 'w';
    if (mode & S_IXOTH) str[8] = 'x';
}

int check_access(const char *role, const char *path, mode_t expected_mode) {
    struct stat file_stat;
    if (stat(path, &file_stat) < 0) {
        return 0; // Fisierul nu exista
    }

    mode_t mode = file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    
    if (strcmp(role, "manager") == 0) {
        if ((mode & S_IRWXU) != (expected_mode & S_IRWXU)) return 0;
    } else if (strcmp(role, "inspector") == 0) {
        if ((mode & S_IRWXG) != (expected_mode & S_IRWXG)) return 0;
    }
    return 1;
}

void setup_district(const char *district) {
    char path[MAX_PATH];
    
    mkdir(district, 0750);
    chmod(district, 0750);

    snprintf(path, sizeof(path), "%s/reports.dat", district);
    int fd = open(path, O_CREAT | O_RDWR, 0664);
    if (fd >= 0)
    { 
        write(fd, "2\n", 0); //Sriem un treshold default de 2.
        close(fd);
    }
    chmod(path, 0640);

    snprintf(path, sizeof(path), "%s/district.cfg", district);
    fd = open(path, O_CREAT | O_RDWR, 0640);
    if (fd >= 0) close(fd);
    chmod(path, 0640);

    snprintf(path, sizeof(path), "%s/logged_district", district);
    fd = open(path, O_CREAT | O_RDWR, 0644);
    if (fd >= 0) close(fd);
    chmod(path, 0644);

    char symlink_name[MAX_PATH];
    snprintf(symlink_name, sizeof(symlink_name), "active_reports-%s", district);
    
    struct stat sym_stat;
    if (lstat(symlink_name, &sym_stat) == -1) {
        symlink(path, symlink_name);
    }
}

void log_action(const char *district, const char *user, const char *role, const char *action) {
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/logged_district", district);

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) return; 

    char log_buffer[256];

    int len = snprintf(log_buffer, sizeof(log_buffer), "%ld\t%s\t%s\t%s\n", 
                       time(NULL), user, role, action);

    write(fd, log_buffer, len);
    close(fd);
}