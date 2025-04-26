CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

# Source files
SRC_ORIGINAL = ../../material/fib.c
SRC_FLATTENED = fib.c

# Default target
all: fib fib_flattened

# Original implementation
fib: $(SRC_ORIGINAL)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Flattened control flow implementation
fib_flattened: $(SRC_FLATTENED)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Clean target to remove compiled files
clean:
	rm -f fib fib_flattened

# Make all targets phony to ensure they always run
.PHONY: all clean