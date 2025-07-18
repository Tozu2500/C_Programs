CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lm

# Source files
SOURCES = main.c physics.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = physics_sim

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Windows build (if using MinGW)
windows: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET).exe $(LIBS)

# Run the simulation
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean windows run
