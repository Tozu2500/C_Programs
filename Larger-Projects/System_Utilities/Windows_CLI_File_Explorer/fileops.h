#ifndef FILEOPS_H
#define FILEOPS_H

#include "types.h"

int loadDirectory(FileList* fileList, const char* path);
void sortFiles(FileList* fileList);
int changeDirectory(FileList* fileList, const char* dirName);
void goUpDirectory(FileList* filelist);
void getCurrentDirectory(char* buffer, int size);
char* formatFileSize(LARGE_INTEGER size, char* buffer);
void formatFileTime(FILETIME ft, char* buffer);

#endif // FILEOPS_H