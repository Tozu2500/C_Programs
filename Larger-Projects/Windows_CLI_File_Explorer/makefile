CC=gcc
CFLAGS=-Wall -Wextra -std=c99
LDFLAGS=-luser32 -lkernel32 -lshell32
TARGET=fileexplorer.exe
SOURCES=main.c console.c fileops.c display.c input.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del *.o $(TARGET)

.PHONY: all clean