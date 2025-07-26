#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

void drawHeader(Console* console, FileList* fileList);
void drawFileList(Console* console, FileList* fileList);
void drawStatusBar(Console* console, FileList* fileList);
void drawHelpBar(Console* console);
void refreshDisplay(Console* console, FileList* fileList);

void setTextColor(Console* console, int attributes);
void resetTextColor(Console* console);
void printAt(Console* console, int x, int y, const char* text);
void clearScreen(Console* console);

#endif // DISPLAY_H