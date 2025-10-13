CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
LDFLAGS = -static
LIBS = -lws2_32

TARGET = file_copier.exe
OBJS = main.o file_operations.o backup_manager.o logger.o threading.o ui_interface.o

HEADERS = file_operations.h backup_manager.h logger.h threading.h ui_interface.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo Build complete: $(TARGET)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

file_operations.o: file_operations.c file_operations.h
	$(CC) $(CFLAGS) -c file_operations.c

backup_manager.o: backup_manager.c backup_manager.h file_operations.h
	$(CC) $(CFLAGS) -c backup_manager.c

logger.o: logger.c logger.h file_operations.h
	$(CC) $(CFLAGS) -c logger.c

threading.o: threading.c threading.h file_operations.h
	$(CC) $(CFLAGS) -c threading.c

ui_interface.o: ui_interface.c ui_interface.h file_operations.h backup_manager.h
	$(CC) $(CFLAGS) -c ui_interface.c

clean:
	del /Q *.o $(TARGET) 2>nul || echo Cleaned

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)

release: CFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

.PHONY: all clean rebuild run debug release