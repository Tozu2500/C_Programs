CC=gcc
CFLAGS=-Wall -Wextra -std=c17 -pedantic -g
TARGET=library_system
SOURCES=main.c book.c library.c utils.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=book.h library.h utils.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)