# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Source and object files
SRCS = main.c ui.c textoperations.c fileoperations.c
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = textproc.exe

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files into .o files
%.o: %.c textproc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
