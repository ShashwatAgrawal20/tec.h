CC = gcc
CFLAGS = -Wall -Wextra
INCLUDES = -Iinclude

SRCDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

TESTDIR = tests
TEST_SRC := $(filter-out $(TEST_RUNNER_SRC), $(shell find $(TESTDIR) -type f -name '*.c'))
TEST_RUNNER_SRC = $(TESTDIR)/test_runner.c
TEST_RUNNER_BIN = $(TESTDIR)/test_runner
TEST_OBJECTS = $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))

TARGET = main

.PHONY: all clean test help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_RUNNER_SRC): $(TEST_SRC)
	@echo 'Generating test runner...'
	@echo '#include "../tec.h"' > $@
	@for file in $(TEST_SRC); do \
		rel_path=$$(echo $$file | sed 's|^$(TESTDIR)/||'); \
		echo "#include \"$$rel_path\"" >> $@; \
	done
	@echo 'TEC_MAIN()' >> $@

$(TEST_RUNNER_BIN): $(TEST_RUNNER_SRC) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(TEST_OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_RUNNER_BIN)
	./$(TEST_RUNNER_BIN)
	@rm -f $(TEST_RUNNER_BIN)

clean:
	rm -rf $(BUILDDIR) $(TARGET) $(TEST_RUNNER_BIN) $(TEST_RUNNER_SRC)

help:
	@echo "Available targets:"
	@echo "  all     - Build main executable"
	@echo "  test    - Build and run tests"
	@echo "  clean   - Clean build artifacts"
	@echo "  help    - Show this help"
