#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "console.h"
#include "fileops.h"
#include "display.h"
#include "input.h"

int main() {
    Console console;
    FileList fileList;
    char currentDir[MAX_PATH_LEN];
    int running = 1;

    initConsole(&console);
    getCurrentDirectory(currentDir, sizeof(currentDir));

    if (!loadDirectory(&fileList, currentDir)) {
        printf("Error! Couldn't load current directory\n");
        return 1;
    }

    while (running) {
        refreshDisplay(&console, &fileList);

        int key = getKey();
        running = handleInput(&console, &fileList, key);
    }

    clearScreen(&console);
    printf("File explorer closed!\n");

    return 0;
}