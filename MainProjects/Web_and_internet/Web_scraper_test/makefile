CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lws2_32
TARGET = webscraper.exe

HEADERS = http_client.h html_parser.h console_ui.h web_scraper.h
SOURCES = main.c http_client.c html_parser.c console_ui.c web_scraper.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o $(TARGET) 2>nul || true

rebuild: clean all

install: $(TARGET)
	copy $(TARGET) C:\Windows\System32\

.PHONY: all clean rebuild install