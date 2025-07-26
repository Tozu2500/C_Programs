#include "console.h"
#include <stdio.h>
#include <conio.h>

void initConsole(Console* console) {
    console->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console->hConsole, &console->csbi);
    console->width = console->csbi.srWindow.Right - console->csbi.srWindow.Left + 1;
    console->height = console->csbi.srWindow.Bottom - console->csbi.srWindow.Top + 1;
}

void clearScreen(Console* console) {
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    DWORD dwConSize;

    dwConSize = console->width * console->height;
    FillConsoleOutputCharacter(console->hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(console->hConsole, &console->csbi);
    FillConsoleOutputAttribute(console->hConsole, console->csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(console->hConsole, coordScreen);
}

void setCursorPosition(Console* console, int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(console->hConsole, coord);
}

void setTextColor(Console* console, int color) {
    SetConsoleTextAttribute(console->hConsole, color);
}

void resetTextColor(Console* console) {
    SetConsoleTextAttribute(console->hConsole, console->csbi.wAttributes);
}

void printAt(Console* console, int x, int y, const char* text) {
    setCursorPosition(console, x, y);
    printf("%s", text);
}

int getKey() {
    int key = _getch();
    if (key == 0 || key == 224) {
        key = _getch();
        switch (key) {
            case 72: return 'U';
            case 80: return 'D';
            case 75: return 'L';
            case 77: return 'R';
        }
    }
    return key;
}