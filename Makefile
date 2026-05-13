CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o commands.o utils.o filter.o
EXEC = city_manager
MONITOR_OBJ = monitor_reports.o
MONITOR_EXEC = monitor_reports

all: $(EXEC) $(MONITOR_EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

$(MONITOR_EXEC): $(MONITOR_OBJ)
	$(CC) $(CFLAGS) -o $(MONITOR_EXEC) $(MONITOR_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC) $(MONITOR_OBJ) $(MONITOR_EXEC)