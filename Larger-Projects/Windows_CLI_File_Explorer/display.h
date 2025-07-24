#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "display.h"

void drawHeader(Console* console, FileList* fileList);
void drawFileList(Console* console, FileList* fileList);
void drawStatusBar(Console* console, FileList* fileList);
void drawHelpBar(Console* console);
void refreshDisplay(Console* console, FileList* fileList);

#endif // DISPLAY_H