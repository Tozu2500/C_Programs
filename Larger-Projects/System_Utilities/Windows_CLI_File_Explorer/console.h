#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"

void initConsole(Console* console);
void clearScreen(Console* console);
void setCursorPosition(Console* console, int x, int y);
void setTextColor(Console* console, int color);
void resetTextColor(Console* console);
void printAt(Console* console, int x, int y, const char* text);
int getKey();

#endif // CONSOLE_H