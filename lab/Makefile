# file: lab/Makefile

CC := gcc
MODE ?= debug

APP_NAME := main_app

SRC_DIR := src
INC_DIR := inc
TEST_DIR := tests

BUILD_ROOT := build
BUILD_DIR := $(BUILD_ROOT)/$(MODE)

APP_TARGET := $(BUILD_DIR)/$(APP_NAME)

APP_SRC := main.c $(wildcard $(SRC_DIR)/*.c)
APP_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(APP_SRC))
APP_DEP := $(APP_OBJ:.o=.d)

TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_SRC))
TEST_DEP := $(TEST_OBJ:.o=.d)

# All production modules under src/ are linked into every test binary.
TEST_DEPS_SRC := $(wildcard $(SRC_DIR)/*.c)
TEST_DEPS_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_DEPS_SRC))
TEST_DEPS_DEP := $(TEST_DEPS_OBJ:.o=.d)

# One executable per test source:
# tests/test_sensor.c -> build/debug/tests/test_sensor
TEST_BINS := $(patsubst %.c,$(BUILD_DIR)/%,$(TEST_SRC))

COMMON_CFLAGS := -Wall -Wextra -Wpedantic -I$(INC_DIR) -MMD -MP
LDFLAGS :=

ifeq ($(MODE),debug)
	CFLAGS := $(COMMON_CFLAGS) -g -O0 -DDEBUG
else ifeq ($(MODE),release)
	CFLAGS := $(COMMON_CFLAGS) -O2 -DNDEBUG
else ifeq ($(MODE),asan)
	CFLAGS := $(COMMON_CFLAGS) -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -DDEBUG
	LDFLAGS += -fsanitize=address,undefined
else
	$(error Unsupported MODE '$(MODE)'. Use MODE=debug|release|asan)
endif

all: $(APP_TARGET)

run: $(APP_TARGET)
	./$(APP_TARGET)

test: $(TEST_BINS)
	@set -e; \
	for t in $(TEST_BINS); do \
		echo "Running $$t"; \
		"$$t"; \
	done

gdb: $(APP_TARGET)
	gdb ./$(APP_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(SRC_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)

$(BUILD_DIR)/$(TEST_DIR):
	mkdir -p $(BUILD_DIR)/$(TEST_DIR)

# Compile top-level .c like main.c
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile src/*.c
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)/$(SRC_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile tests/*.c
$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)/$(TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the main app
$(APP_TARGET): $(APP_OBJ)
	$(CC) $(APP_OBJ) $(LDFLAGS) -o $@

# Build one test binary per test source file
# Example:
#   tests/test_msg_store.c
# becomes:
#   build/debug/tests/test_msg_store
$(BUILD_DIR)/$(TEST_DIR)/%: $(BUILD_DIR)/$(TEST_DIR)/%.o $(TEST_DEPS_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILD_ROOT)

-include $(APP_DEP)
-include $(TEST_DEP)
-include $(TEST_DEPS_DEP)

.PHONY: all run test gdb clean