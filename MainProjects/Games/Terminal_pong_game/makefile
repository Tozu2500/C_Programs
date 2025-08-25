CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = pong
SRCDIR = .
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADERS = $(wildcard $(SRCDIR)/*.h)

.PHONY: all clean run debug release install uninstall

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
	rm -f *.exe *.o

run: $(BINDIR)/$(TARGET)
	$(BINDIR)/$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean $(BINDIR)/$(TARGET)

release: CFLAGS += -DNDEBUG
release: clean $(BINDIR)/$(TARGET)

install: $(BINDIR)/$(TARGET)
	cp $(BINDIR)/$(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

rebuild: clean all

test: $(BINDIR)/$(TARGET)
	$(BINDIR)/$(TARGET)

help:
	@echo "Available targets:"
	@echo "  all      - Build the game (default)"
	@echo "  clean    - Remove all build files"
	@echo "  run      - Build and run the game"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  install  - Install to system"
	@echo "  uninstall- Remove from system"
	@echo "  rebuild  - Clean and build"
	@echo "  test     - Build and test the game"
	@echo "  help     - Show this help message"