#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <windows.h>
#include <stdio.h>

typedef struct {
    char name[MAX_PATH];
    char extension[32];
    DWORD size_high;
    DWORD size_low;
    FILETIME creation_time;
    FILETIME modified_time;
    FILETIME access_time;
    DWORD attributes;
    int is_directory;
    int is_hidden;
    int is_readonly;
    int is_system;
} FileInfo;

int file_exists(const char* filepath);
int get_file_info(const char* filepath, FileInfo* info);
void format_file_size(DWORD size_high, DWORD size_low, char* buffer, int buffer_size);
void format_file_time(FILETIME* ft, char* buffer, int buffer_size);
const char* get_file_type_description(const char* extension);

#endif // FILE_UTILS_H
