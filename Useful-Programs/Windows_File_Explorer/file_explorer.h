#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MAX_PATH_LEN 260
#define MAX_FILES 1000

// Struct for file entry
typedef struct {
    char name[MAX_PATH_LEN];
    char full_path[MAX_PATH_LEN];
    DWORD attributes;
    LARGE_INTEGER file_size;
    FILETIME last_modified;
    int is_directory;
} FileEntry;

// Struct for file explorer
typedef struct {
    char current_path[MAX_PATH_LEN];
    FileEntry files[MAX_FILES];
    int file_count;
} FileExplorer;

// Function protos
void init_explorer(FileExplorer *explorer);
int list_directory(FileExplorer *explorer, const char *path);
void display_files(const FileExplorer *explorer);
void display_file_info(const FileEntry *file);
int change_directory(FileExplorer *explorer, const char *dir_name);
int go_parent_directory(FileExplorer *explorer);
void format_file_size(LARGE_INTEGER size, char *buffer);
void format_file_time(FILETIME *ft, char *buffer);
void clear_screen(void);
void show_help(void);
int is_valid_path(const char *path);

#endif // FILE_EXPLORER_H
