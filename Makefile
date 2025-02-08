# This shit is just for testing purposes.
CC = clang
CFLAGS = -Wall -Wextra

TARGET = tec
SOURCES = ./tec.c ./ex_test_utility_fn/math_utils.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all clean
clean:
	rm -f $(TARGET)

