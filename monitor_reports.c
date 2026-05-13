#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

volatile sig_atomic_t running = 1;

void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

void sigusr1_handler(int sig) {
    (void)sig;
    printf("New report added.\n");
}

int main() {
    
    pid_t pid = getpid();
    int fd = open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create .monitor_pid");
        return 1;
    }
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "%d\n", pid);
    write(fd, buf, len);
    close(fd);

    // Set up signal handlers
    struct sigaction sa_int, sa_usr1;
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    printf("Monitor started. PID: %d\n", pid);

    while (running) {
        pause();
    }

    printf("Monitor ending.\n");
    unlink(".monitor_pid");
    return 0;
}