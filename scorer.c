#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "report.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <district_path>\n", argv[0]);
        return 1;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/reports.dat", argv[1]);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "District [%s]: Nu s-a putut deschide reports.dat\n", argv[1]);
        return 1;
    }

    Report r;
    while (read(fd, &r, sizeof(Report)) > 0) {
        printf("Inspector: %s | Score: %d\n", r.inspector_name, r.severity_level);
    }

    close(fd);
    return 0;
}
