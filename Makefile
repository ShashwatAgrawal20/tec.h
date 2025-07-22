CC = gcc
CFLAGS = -Wall -Wextra -pedantic
INCLUDES = -Iinclude

SRCDIR = src
TESTDIR = tests
BUILDDIR = build

SRC_BUILDDIR = $(BUILDDIR)/src
TEST_BUILDDIR = $(BUILDDIR)/test

TARGET = main
TEST_RUNNER_BIN = test_runner
TEST_RUNNER_SRC := $(TESTDIR)/test_runner.c

SRC_FILES := $(wildcard $(SRCDIR)/*.c)
TEST_SRC_FILES := $(shell find $(TESTDIR) -type f -name '*.c')

SRC_OBJECTS := $(patsubst $(SRCDIR)/%.c,$(SRC_BUILDDIR)/%.o,$(SRC_FILES))
TEST_OBJECTS := $(patsubst $(TESTDIR)/%.c,$(TEST_BUILDDIR)/%.o,$(TEST_SRC_FILES))

NON_MAIN_OBJECTS := $(filter-out $(SRC_BUILDDIR)/main.o, $(SRC_OBJECTS))

.PHONY: all clean test help

all: $(TARGET)

$(TARGET): $(SRC_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_RUNNER_BIN): $(TEST_OBJECTS) $(NON_MAIN_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(SRC_BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_BUILDDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_RUNNER_BIN)
	@echo "Running tests..."
	./$(TEST_RUNNER_BIN)

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(TARGET) $(TEST_RUNNER_BIN)

help:
	@echo "Available targets:"
	@echo "  all           - Build main executable ($(TARGET))"
	@echo "  test          - Build and run tests"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
