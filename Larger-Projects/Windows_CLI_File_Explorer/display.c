#include "display.h"
#include "fileops.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

void drawHeader(Console* console, FileList* fileList) {
    setTextColor(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);

    char header[512];
    sprintf(header, " File Explorer - %s", fileList->currentPath);

    for (int i = strlen(header); i < console->width; i++) {
        strcat(header, " ");
    }

    printAt(console, 0, 0, header);
    resetTextColor(console);
}

void drawFileList(Console* console, FileList* fileList) {
    int startLine = 2;
    int maxLines = console->height - 5;
    int startIndex = 0;

    if (fileList->selected >= maxLines) {
        startIndex = fileList->selected - maxLines + 1;
    }

    for (int i = 0; i < maxLines && (startIndex + i) < fileList->count; i++) {
        int fileIndex = startIndex + i;
        FileItem* file = &fileList->files[fileIndex];

        char line[512];
        char sizeStr[32];
        char timeStr[32];

        if (file->isDirectory) {
            strcpy(sizeStr, "<DIR>");
        } else {
            formatFileSize(file->size, sizeStr);
        }

        formatFileTime(file->lastWrite, timeStr);

        sprintf(line, " %-30s %10s  %s", file->name, sizeStr, timeStr);

        for (int j = strlen(line); j < console->width - 1; j++) {
            strcat(line, " ");
        }

        if (fileIndex == fileList->selected) {
            setTextColor(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
        } else if (file->isDirectory) {
            setTextColor(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }

        printAt(console, 0, startLine + i, line);
        resetTextColor(console);
    }

    for (int i = fileList->count - startIndex; i < maxLines; i++) {
        char emptyLine[512] = "";
        for (int j = 0; j < console->width - 1; j++) {
            strcat(emptyLine, " ");
        }
        printAt(console, 0, startLine + i, emptyLine);
    }
}

void drawStatusBar(Console* console, FileList* fileList) {
    int statusY = console->height - 2;

    setTextColor(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);

    char status[512];
    sprintf(status, " %d files | Selected: %s",
        fileList->count,
        fileList->count > 0 ? fileList->files[fileList->selected].name : "None");
    
    for (int i = strlen(status); i < console->width; i++) {
        strcat(status, " ");
    }

    printAt(console, 0, statusY, status);
    resetTextColor(console);
}

void drawHelpBar(Console* console) {
    int helpY = console->height - 1;

    setTextColor(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN);

    char help[512] = " ENTER: Open | BACKSPACE: Up | ESC: Quit | Arrow Keys: Navigate";

    for (int i = strlen(help); i < console->width; i++) {
        strcat(help, " ");
    }

    printAt(console, 0, helpY, help);
    resetTextColor(console);
}

void refreshDisplay(Console* console, FileList* fileList) {
    clearScreen(console);
    drawHeader(console, fileList);
    drawFileList(console, fileList);
    drawStatusBar(console, fileList);
    drawHelpBar(console);
}