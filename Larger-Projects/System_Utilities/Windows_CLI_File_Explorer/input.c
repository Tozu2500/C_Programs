#include "input.h"
#include "fileops.h"
#include <stdio.h>
#include <windows.h>

int handleInput(Console* console, FileList* fileList, int key) {
    (void)console;
    
    switch (key) {
        case 'U':
            navigateUp(fileList);
            return 1;
        case 'D':
            navigateDown(fileList);
            return 1;
        case 13:
            return openSelected(fileList);
        case 8:
            return goBack(fileList);
        case 27:
            return 0;
        default:
            return 1;
    }
}

void navigateUp(FileList* fileList) {
    if (fileList->selected > 0) {
        fileList->selected--;
    }
}

void navigateDown(FileList* fileList) {
    if (fileList->selected < fileList->count - 1) {
        fileList->selected++;
    }
}

int openSelected(FileList* fileList) {
    if (fileList->count == 0) return 1;

    FileItem* selected = &fileList->files[fileList->selected];

    if (selected->isDirectory) {
        return changeDirectory(fileList, selected->name);
    } else {
        char fullPath[MAX_PATH_LEN];
        int result = snprintf(fullPath, sizeof(fullPath), "%s\\%s", 
                             fileList->currentPath, selected->name);
        
        if (result >= (int)sizeof(fullPath)) {
            printf("Error: Path too long\n");
            return 1;
        }
        
        ShellExecute(NULL, "open", fullPath, NULL, NULL, SW_SHOWNORMAL);
        return 1;
    }
}

int goBack(FileList* fileList) {
    return changeDirectory(fileList, "..");
}