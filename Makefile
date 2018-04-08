# Declaration of variables
CC=gcc
CC_FLAGS=-W -Wall -Wextra -Werror -g -std=gnu11

# File names
EXEC=run
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

