CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS =

# Source files
SRC_ORIGINAL = ../../material/fib.c
SRC_VM = vm_interpreter.c

# Default target
all: fib fib_vm

# Original implementation
fib: $(SRC_ORIGINAL)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

fib_vm: $(SRC_VM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Clean target to remove compiled files
clean:
	rm -f fib_vm

# Make all targets phony to ensure they always run
.PHONY: all clean