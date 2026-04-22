#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

typedef struct {
    int id;
    char district[32];
    char issue_type[32];
    int severity;
    long timestamp;
    char details[128];
    int active;
} Report;

void add_report(const char *district, const char *role) {
    if (strcmp(role, "inspector") != 0 && strcmp(role, "manager") != 0) {
        return;
    }

    char filename[64];
    snprintf(filename, sizeof(filename), "%s.bin", district);

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) return;

    Report r;
    r.id = (int)time(NULL);
    strncpy(r.district, district, 31);
    strncpy(r.issue_type, "General Issue", 31);
    r.severity = 1;
    r.timestamp = time(NULL);
    strncpy(r.details, "Standard report", 127);
    r.active = 1;

    write(fd, &r, sizeof(Report));
    close(fd);
}

void remove_report(const char *district, int index, const char *role) {
    if (strcmp(role, "manager") != 0) return;

    char filename[64];
    snprintf(filename, sizeof(filename), "%s.bin", district);

    int fd = open(filename, O_RDWR);
    if (fd == -1) return;

    Report r;
    off_t offset = index * sizeof(Report);
    
    if (lseek(fd, offset, SEEK_SET) != -1) {
        if (read(fd, &r, sizeof(Report)) > 0) {
            r.active = 0;
            lseek(fd, offset, SEEK_SET);
            write(fd, &r, sizeof(Report));
        }
    }
    close(fd);
}


int main(int argc, char *argv[])
{
    char *role = NULL;
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i], "--role") == 0 && i+1 < argc) {
            role = argv[i+1];
        }
        else if(strcmp(argv[i], "--add") == 0 && i+1 < argc){
            if (role) add_report(argv[++i], role);
        } else if (strcmp(argv[i], "--remove_report") == 0 && i + 2 < argc) {
            if (role) {
                char *dist = argv[++i];
                int idx = atoi(argv[++i]);
                remove_report(dist, idx, role);
            }
            
        }
    }
    printf("Operation completed.\n");
    return 0;
}