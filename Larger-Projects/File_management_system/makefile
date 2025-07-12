# Makefile for File Management System

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = file_manager
SOURCES = main.c file_manager.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = file_manager.h

# Windows-specific settings
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
    MKDIR = mkdir
    RMDIR = rmdir /S /Q
else
    RM = rm -f
    MKDIR = mkdir -p
    RMDIR = rm -rf
endif

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Build object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	$(RM) $(OBJECTS) $(TARGET)

# Install (optional, Unix-like systems only)
install: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo "Install not supported on Windows"
else
	cp $(TARGET) /usr/local/bin/
endif

# Uninstall (optional, Unix-like systems only)
uninstall:
ifeq ($(OS),Windows_NT)
	@echo "Uninstall not supported on Windows"
else
	rm -f /usr/local/bin/$(TARGET)
endif

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Check for memory leaks (requires valgrind, Unix-like systems only)
valgrind: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo "Valgrind not available on Windows"
else
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
endif

# Create a test directory structure
test-setup:
	$(MKDIR) test_dir
ifeq ($(OS),Windows_NT)
	$(MKDIR) test_dir\\subdir1
	$(MKDIR) test_dir\\subdir2
	echo Hello World > test_dir\\hello.txt
	echo This is a test file > test_dir\\test.txt
	echo Another file > test_dir\\subdir1\\file1.txt
else
	$(MKDIR) test_dir/subdir1 test_dir/subdir2
	echo "Hello World" > test_dir/hello.txt
	echo "This is a test file" > test_dir/test.txt
	echo "Another file" > test_dir/subdir1/file1.txt
endif

# Clean test directory
test-clean:
	$(RMDIR) test_dir

.PHONY: all clean install uninstall debug run valgrind test-setup test-clean