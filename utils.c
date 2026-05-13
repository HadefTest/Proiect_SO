#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

void mode_to_symbolic(mode_t mode, char *buf) {
    buf[0] = (mode & S_IRUSR) ? 'r' : '-';
    buf[1] = (mode & S_IWUSR) ? 'w' : '-';
    buf[2] = (mode & S_IXUSR) ? 'x' : '-';
    buf[3] = (mode & S_IRGRP) ? 'r' : '-';
    buf[4] = (mode & S_IWGRP) ? 'w' : '-';
    buf[5] = (mode & S_IXGRP) ? 'x' : '-';
    buf[6] = (mode & S_IROTH) ? 'r' : '-';
    buf[7] = (mode & S_IWOTH) ? 'w' : '-';
    buf[8] = (mode & S_IXOTH) ? 'x' : '-';
    buf[9] = '\0';
}

int check_permission(const char *path, mode_t required_bit) {
    struct stat st;
    if (stat(path, &st) < 0) return 1; 
    return (st.st_mode & required_bit) ? 1 : 0;
}

void write_log(const char *district, const char *user, const char *role, const char *action, int monitor_notified) {
    char log_path[512];
    snprintf(log_path, sizeof(log_path), "%s/logged_district", district);
    
    int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return;
    chmod(log_path, 0644);

    char buf[1024];
    int len;
    if (monitor_notified == 1) {
        len = snprintf(buf, sizeof(buf), "%ld %s %s %s (Monitor notified via SIGUSR1)\n", time(NULL), user, role, action);
    } else {
        len = snprintf(buf, sizeof(buf), "%ld %s %s %s (Monitor NOT notified: offline/error)\n", time(NULL), user, role, action);
    }
    write(fd, buf, len);
    close(fd);
}

int notify_monitor(void) {
    int fd = open(".monitor_pid", O_RDONLY);
    if (fd < 0) return 0;
    
    char pid_buf[32];
    memset(pid_buf, 0, sizeof(pid_buf));
    int bytes = read(fd, pid_buf, sizeof(pid_buf) - 1);
    close(fd);
    
    if (bytes <= 0) return 0;
    pid_t pid = (pid_t)atoi(pid_buf);
    if (pid <= 0) return 0;
    
    if (kill(pid, SIGUSR1) == 0) return 1;
    return 0;
}