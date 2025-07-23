#ifndef TYPES_H
#define TYPES_H

#include <windows.h>

#define MAX_PATH_LEN 260
#define MAX_FILES 1000
#define MAX_NAME_LEN 256

typedef struct {
    char name[MAX_NAME_LEN];
    DWORD attributes;
    LARGE_INTEGER size;
    FILETIME lastWrite;
    int isDirectory;
} FileItem;

typedef struct {
    FileItem files[MAX_FILES];
    int count;
    int selected;
    char currentPath[MAX_PATH_LEN];
} FileList;

typedef struct {
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width;
    int height;
} Console;

#endif // TYPES_H