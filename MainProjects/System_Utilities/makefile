CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lpsapi
TARGET = sysmon
SRCDIR = .
SOURCES = main.c system_info.c process_info.c ui.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = sysmon.h system_info.h process_info.h ui.HEADERS

.PHONY: all clean install debug release

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
# BAT COMPILATIONS SUCK
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

release: CFLAGS += -DNDEBUG
release: $(TARGET)

clean:
	del /Q *.o $(TARGET).exe 2>nul || echo Clean completed

install: $(TARGET)
	copy $(TARGET).exe C:\Windows\System32\

run: $(TARGET)
	.\$(TARGET).exe

help:
	@echo Available targets:
	@echo	all     - Build the system monitor (default)
	@echo   debug   - Build with debug information
	@echo   release - Build optimized release version
	@echo   clean   - Remove object files and executable
	@echo   install - Install to system directory
	@echo   run     - Build and run the program
	@echo   help    - Show this help message 