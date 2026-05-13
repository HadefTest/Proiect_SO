CC = gcc
CFLAGS = -Wall -Wextra -g

all: city_manager monitor_reports

city_manager: main.o commands.o filter.o utils.o
	$(CC) $(CFLAGS) -o city_manager main.o commands.o filter.o utils.o

monitor_reports: monitor_reports.o
	$(CC) $(CFLAGS) -o monitor_reports monitor_reports.o

main.o: main.c commands.h
	$(CC) $(CFLAGS) -c main.c

commands.o: commands.c commands.h report.h filter.h utils.h
	$(CC) $(CFLAGS) -c commands.c

filter.o: filter.c filter.h report.h
	$(CC) $(CFLAGS) -c filter.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

monitor_reports.o: monitor_reports.c
	$(CC) $(CFLAGS) -c monitor_reports.c

clean:
	rm -f *.o city_manager monitor_reports .monitor_pid active_reports-*