#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void execute_start_monitor() {
    int pfd[2];
    if (pipe(pfd) < 0) {
        perror("Eroare la crearea pipe-ului");
        return;
    }

    pid_t hub_mon_pid = fork();
    if (hub_mon_pid < 0) {
        perror("Eroare la fork");
        return;
    }

    if (hub_mon_pid == 0) {
        // Suntem în procesul intermediar (hub_mon)
        close(pfd[0]); // Închidem capătul de citire, nu avem nevoie de el aici

        // Pasăm capătul de scriere prin variabila de mediu MONITOR_PIPE_FD
        char fd_str[16];
        snprintf(fd_str, sizeof(fd_str), "%d", pfd[1]);
        setenv("MONITOR_PIPE_FD", fd_str, 1);

        // Lansăm monitorul
        char *args[] = {"./monitor_reports", NULL};
        execvp(args[0], args);
        
        perror("Execvp a esuat pentru monitor_reports");
        exit(1);
    }

    // Suntem în procesul părinte (city_hub)
    close(pfd[1]); // Închidem capătul de scriere în părinte

    printf("[Hub] Procesul monitor a fost pornit de hub_mon.\n");
    
    // Citim în buclă din pipe mesajele venite de la monitor
    char buffer[512];
    ssize_t n;
    while ((n = read(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("[Monitor Output]: %s", buffer);
        
        // Dacă monitorul anunță că se oprește sau rulează deja altul
        if (strstr(buffer, "ERR_ALREADY_RUNNING") || strstr(buffer, "MONITOR_EXIT")) {
            printf("[Hub] Monitorul s-a oprit. Inchidem conexiunea pipe.\n");
            break;
        }
    }
    close(pfd[0]);
}

void execute_calculate_scores(char *districts[], int count) {
    int pipes[50][2];
    pid_t pids[50];

    // Pornim fiecare scorer în paralel
    for (int i = 0; i < count; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Pipe esuat");
            return;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Fork esuat");
            return;
        }

        if (pids[i] == 0) {
            // Procesul Fiu (Scorer)
            close(pipes[i][0]); // Închidem citirea din fiu

            // Redirecționăm stdout către capătul de scriere al pipe-ului
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][1]);

            char *args[] = {"./scorer", districts[i], NULL};
            execvp(args[0], args);

            perror("Execvp esuat pentru scorer");
            exit(1);
        }
        
        // În părinte închidem scrierea pentru acest fiu
        close(pipes[i][1]);
    }

    // Colectăm și printăm rezultatele trimise prin pipe-uri
    printf("\n=== COLECTARE REZULTATE WORKLOAD ===\n");
    for (int i = 0; i < count; i++) {
        char buffer[1024];
        ssize_t n;
        while ((n = read(pipes[i][0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }
        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0); // Curățăm procesul zombie
    }
    printf("====================================\n\n");
}

int main() {
    char line[1024];
    printf("Sistem City Hub pornit. Introduceti comenzi (start_monitor, calculate_scores <districte>, exit).\n");

    while (1) {
        printf("city_hub> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // Eliminăm caracterul newline
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, " ");
        if (!token) continue;

        if (strcmp(token, "exit") == 0) {
            break;
        } 
        else if (strcmp(token, "start_monitor") == 0) {
            execute_start_monitor();
        } 
        else if (strcmp(token, "calculate_scores") == 0) {
            char *districts[50];
            int d_count = 0;
            while ((token = strtok(NULL, " ")) != NULL && d_count < 50) {
                districts[d_count++] = token;
            }
            if (d_count == 0) {
                printf("Specificati cel putin un district.\n");
            } else {
                execute_calculate_scores(districts, d_count);
            }
        } 
        else {
            printf("Comanda necunoscuta.\n");
        }
    }
    return 0;
}