CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o commands.o utils.o filter.o
EXEC = city_manager

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)