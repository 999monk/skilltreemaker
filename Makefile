CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

TARGET  = skilltree
SRC     = main.c src/graph.c src/skilltree.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
