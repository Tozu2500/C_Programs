CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = main.o file_utils.o display.o input_handler.o security.o

# The output executable name
TARGET = file_metadata_viewer.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c file_utils.h display.h input_handler.h security.h

file_utils.o: file_utils.c file_utils.h

display.o: display.c display.h file_utils.h

input_handler.o: input_handler.c input_handler.h

security.o: security.c security.h file_utils.h

clean:
	del /Q *.o *.exe 2>nul || true

.PHONY: all clean