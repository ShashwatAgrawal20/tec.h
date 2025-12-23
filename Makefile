CC = gcc
CFLAGS = -Wall -Wextra -pedantic
INCLUDES = -Iinclude

SRCDIR = src
TESTDIR = tests
BUILDDIR = build

ifeq ($(OS),Windows_NT)
	RM = cmd /C del /Q /F
	RMDIR = cmd /C rmdir /S /Q
	MKDIR = cmd /C if not exist
	TEST_SRC_FILES := $(shell dir /S /B $(TESTDIR)\*.c 2>nul)
else
	RM = rm -f
	RMDIR = rm -rf
	MKDIR = mkdir -p
	TEST_SRC_FILES := $(shell find $(TESTDIR) -type f -name '*.c')
endif

TARGET = main
TEST_RUNNER_BIN = $(TESTDIR)/test_runner
TEST_RUNNER_SRC := $(TESTDIR)/test_runner.c

SRC_FILES := $(wildcard $(SRCDIR)/*.c)

SRC_OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRC_FILES))
TEST_OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.o,$(TEST_SRC_FILES))

NON_MAIN_OBJECTS := $(filter-out $(BUILDDIR)/$(SRCDIR)/main.o, $(SRC_OBJECTS))

.PHONY: all clean test help

all: $(TARGET)

$(TARGET): $(SRC_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_RUNNER_BIN): $(TEST_OBJECTS) $(NON_MAIN_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(BUILDDIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_RUNNER_BIN)
	@echo "Running tests..."
	./$(TEST_RUNNER_BIN)

workflow_test: $(TEST_RUNNER_BIN)
	@echo "Running tests..."
	./$(TEST_RUNNER_BIN) -f !this_xfail_should_unexpectedly_pass_and_fail_the_suite

clean:
	@echo "Cleaning build artifacts..."
	-@$(RMDIR) $(BUILDDIR)
	-@$(RM) $(TARGET)
	-@$(RM) $(TEST_RUNNER_BIN)

help:
	@echo "Available targets:"
	@echo "  all           - Build main executable ($(TARGET))"
	@echo "  test          - Build and run tests"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
