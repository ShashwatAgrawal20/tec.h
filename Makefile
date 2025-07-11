CC = gcc
CFLAGS = -Wall -Wextra
INCLUDES = -Iinclude

SRCDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

TESTDIR = tests
TEST_RUNNER = $(TESTDIR)/test_runner
TEST_OBJECTS = $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))

TARGET = main

.PHONY: all clean test help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_RUNNER): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(TESTDIR)/test_runner.c $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

clean:
	rm -rf $(BUILDDIR) $(TARGET) $(TEST_RUNNER)

help:
	@echo "Available targets:"
	@echo "  all     - Build main executable"
	@echo "  test    - Build and run tests"
	@echo "  clean   - Clean build artifacts"
	@echo "  help    - Show this help"
