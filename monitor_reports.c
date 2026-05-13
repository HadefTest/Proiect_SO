#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int hub_pipe_fd = -1;

void handle_sigint(int sig) {
    (void)sig;
    if (hub_pipe_fd >= 0) {
        write(hub_pipe_fd, "MONITOR_EXIT: Sesiune terminata prin SIGINT.\n", 45);
        close(hub_pipe_fd);
    } else {
        write(STDOUT_FILENO, "\nMonitor session terminated safely via SIGINT.\n", 47);
    }
    unlink(".monitor_pid");
    exit(0);
}

void handle_sigusr1(int sig) {
    (void)sig;
    if (hub_pipe_fd >= 0) {
        write(hub_pipe_fd, "MONITOR_ALERT: Un nou raport a fost adaugat.\n", 45);
    } else {
        write(STDOUT_FILENO, "[ALERT] A new report has been committed to infrastructure files.\n", 65);
    }
}

int main() {
    char *pipe_fd_env = getenv("MONITOR_PIPE_FD");
    if (pipe_fd_env) {
        hub_pipe_fd = atoi(pipe_fd_env);
    }

    int check_fd = open(".monitor_pid", O_RDONLY);
    if (check_fd >= 0) {
        char old_pid_str[16];
        ssize_t n = read(check_fd, old_pid_str, sizeof(old_pid_str) - 1);
        close(check_fd);
        
        if (n > 0) {
            old_pid_str[n] = '\0';
            char err_msg[128];
            snprintf(err_msg, sizeof(err_msg), "ERR_ALREADY_RUNNING: Un monitor ruleaza deja cu PID %s\n", old_pid_str);
            
            if (hub_pipe_fd >= 0) {
                write(hub_pipe_fd, err_msg, strlen(err_msg));
                close(hub_pipe_fd);
            } else {
                fprintf(stderr, "%s", err_msg);
            }
            return 1;
        }
    }

    int fd = open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to link PID file");
        if (hub_pipe_fd >= 0) close(hub_pipe_fd);
        return 1;
    }

    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    write(fd, pid_str, strlen(pid_str));
    close(fd);

    struct sigaction sa_int, sa_usr1;

    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa_int, NULL);

    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    if (hub_pipe_fd >= 0) {
        write(hub_pipe_fd, "MONITOR_START: Sistemul de monitorizare este online.\n", 53);
    } else {
        printf("Monitor system online. Active PID: %d. Processing events...\n", getpid());
    }

    while (1) {
        pause(); 
    }

    return 0;
}