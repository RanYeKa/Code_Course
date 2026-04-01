# file name and root related path: remidner/Makefile

CC := gcc
CFLAGS := -Wall -Wextra -g -Iinc

BUILD_DIR := build

APP_NAME := main_app
APP_TARGET := $(BUILD_DIR)/$(APP_NAME)

TEST_NAME := test_msg_store
TEST_TARGET := $(BUILD_DIR)/$(TEST_NAME)

APP_SRC := main.c $(wildcard src/*.c)
APP_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(APP_SRC))

TEST_SRC := tests/test_msg_store.c
TEST_OBJ := $(BUILD_DIR)/tests/test_msg_store.o

# Only the sources needed by the test executable
TEST_DEPS_SRC := src/msg_store.c src/sensor.c
TEST_DEPS_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_DEPS_SRC))

all: $(APP_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/src:
	mkdir -p $(BUILD_DIR)/src

$(BUILD_DIR)/tests:
	mkdir -p $(BUILD_DIR)/tests

# Generic compile rule for top-level .c files like main.c
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for src/*.c
$(BUILD_DIR)/src/%.o: src/%.c | $(BUILD_DIR)/src
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for tests/*.c
$(BUILD_DIR)/tests/%.o: tests/%.c | $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_TARGET): $(APP_OBJ)
	$(CC) $(APP_OBJ) -o $@

$(TEST_TARGET): $(TEST_OBJ) $(TEST_DEPS_OBJ)
	$(CC) $(TEST_OBJ) $(TEST_DEPS_OBJ) -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean