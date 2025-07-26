#include "fileops.h"
#include <string.h>
#include <stdio.h>

int loadDirectory(FileList* fileList, const char* path) {
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char searchPath[MAX_PATH_LEN];

    strcpy(fileList->currentPath, path);
    sprintf(searchPath, "%s\\*", path);

    fileList->count = 0;
    fileList->selected = 0;

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    do {
        if (fileList->count >= MAX_FILES) break;

        strcpy(fileList->files[fileList->count].name, findData.cFileName);
        fileList->files[fileList->count].attributes = findData.dwFileAttributes;
        fileList->files[fileList->count].size.LowPart = findData.nFileSizeLow;
        fileList->files[fileList->count].size.HighPart = findData.nFileSizeHigh;
        fileList->files[fileList->count].lastWrite = findData.ftLastWriteTime;
        fileList->files[fileList->count].isDirectory =
            (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        fileList->count++;

    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    sortFiles(fileList);
    return 1;
}

void sortFiles(FileList* fileList) {
    for (int i = 0; i < fileList->count - 1; i++) {
        for (int j = 0; j < fileList->count - i - 1; j++) {
            int swap = 0;

            if (fileList->files[j].isDirectory && !fileList->files[j + 1].isDirectory) {
                swap = 0;
            } else if (!fileList->files[j].isDirectory && fileList->files[j + 1].isDirectory) {
                swap = 1;
            } else {
                swap = strcmp(fileList->files[j].name, fileList->files[j + 1].name) > 0;
            }

            if (swap) {
                FileItem temp = fileList->files[j];
                fileList->files[j] = fileList->files[j + 1];
                fileList->files[j + 1] = temp;
            }
        }
    }
}

int changeDirectory(FileList* fileList, const char* dirName) {
    char newPath[MAX_PATH_LEN];

    if (strcmp(dirName, "..") == 0) {
        goUpDirectory(fileList);
        return loadDirectory(fileList, fileList->currentPath);
    }

    sprintf(newPath, "%s\\%s", fileList->currentPath, dirName);
    return loadDirectory(fileList, newPath);
}

void goUpDirectory(FileList* fileList) {
    char* lastSlash = strrchr(fileList->currentPath, '\\');
    if (lastSlash && lastSlash != fileList->currentPath + 2) {
        *lastSlash = '\0';
    }
}

void getCurrentDirectory(char* buffer, int size) {
    GetCurrentDirectory(size, buffer);
}

char* formatFileSize(LARGE_INTEGER size, char* buffer) {
    double bytes = (double)size.QuadPart;

    if (bytes < 1024) {
        sprintf(buffer, "%.0f B", bytes);
    } else if (bytes < 1024 * 1024) {
        sprintf(buffer, "%.1f KB", bytes / 1024);
    } else if (bytes < 1024 * 1024 * 1024) {
        sprintf(buffer, "%.1f MB", bytes / (1024 * 1024));
    } else {
        sprintf(buffer, "%.1f GB", bytes / (1024 * 1024 * 1024));
    }

    return buffer;
}

void formatFileTime(FILETIME ft, char* buffer) {
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d",
            st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute);
}