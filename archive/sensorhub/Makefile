# file name and root related path: sensorhub/Makefile

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
	rm -f $(OBJ) $(TARGET)
