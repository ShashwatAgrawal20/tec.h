# This shit is just for testing purposes.
CC = clang
CFLAGS = -Wall -Wextra

TARGET = tec_test
SOURCES = ./tec.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all clean
clean:
	rm -f $(TARGET)

