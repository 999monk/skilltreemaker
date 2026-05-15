CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

TARGET  = skilltree
SRC     = main.c src/graph.c src/cli.c src/persist.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
