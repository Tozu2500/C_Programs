
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -g -O2

# Directories
OBJDIR = obj
BINDIR = bin

# Files
TARGET = $(BINDIR)/Student_Record_System.exe
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJS) -o $@

# Compile .c files into .o files
$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
