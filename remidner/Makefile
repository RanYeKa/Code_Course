CC = gcc
CFLAGS = -Wall -Iinc
SRC = main.c $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = main_app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)

TEST_TARGET = test_msg_store

$(TEST_TARGET): src/msg_store.c src/sensor.c tests/test_msg_store.c
	$(CC) $(CFLAGS) -Wall -Wextra -g $^ -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)
