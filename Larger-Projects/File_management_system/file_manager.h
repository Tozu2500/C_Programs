#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define MAX_PATH_LENGTH 1024
#define MAX_FILENAME_LENGTH 256
#define BUFFER_SIZE 4096

// File operation result codes
typedef enum {
    FILE_OP_SUCCESS = 0,
    FILE_OP_ERROR = -1,
    FILE_OP_NOT_FOUND = -2,
    FILE_OP_PERMISSION_DENIED = -3,
    FILE_OP_ALREADY_EXISTS = -4,
    FILE_OP_INVALID_PATH = -5
} FileOpResult;

// File information structure
typedef struct {
    char name[MAX_FILENAME_LENGTH];
    char path[MAX_PATH_LENGTH];
    long size;
    time_t modified_time;
    int is_directory;
    mode_t permissions;
} FileInfo;

// Directory listing structure
typedef struct {
    FileInfo *files;
    int count;
    int capacity;
} DirectoryListing;

// Core file operations
FileOpResult fm_create_file(const char *filepath, const char *content);
FileOpResult fm_read_file(const char *filepath, char **content, long *size);
FileOpResult fm_write_file(const char *filepath, const char *content, int append);
FileOpResult fm_delete_file(const char *filepath);
FileOpResult fm_copy_file(const char *source, const char *destination);
FileOpResult fm_move_file(const char *source, const char *destination);
FileOpResult fm_rename_file(const char *old_path, const char *new_path);

// Directory operations
FileOpResult fm_create_directory(const char *dirpath);
FileOpResult fm_delete_directory(const char *dirpath);
FileOpResult fm_list_directory(const char *dirpath, DirectoryListing *listing);
FileOpResult fm_change_directory(const char *dirpath);
char* fm_get_current_directory(void);

// File information and utilities
FileOpResult fm_get_file_info(const char *filepath, FileInfo *info);
int fm_file_exists(const char *filepath);
int fm_is_directory(const char *filepath);
long fm_get_file_size(const char *filepath);
FileOpResult fm_set_permissions(const char *filepath, mode_t permissions);

// Search and filtering
FileOpResult fm_search_files(const char *directory, const char *pattern, DirectoryListing *results);
FileOpResult fm_filter_by_extension(const DirectoryListing *input, const char *extension, DirectoryListing *output);

// Utility functions
void fm_free_directory_listing(DirectoryListing *listing);
const char* fm_get_error_message(FileOpResult result);
char* fm_get_file_extension(const char *filename);
void fm_print_file_info(const FileInfo *info);
void fm_print_directory_listing(const DirectoryListing *listing);

// Path manipulation
char* fm_join_path(const char *dir, const char *filename);
char* fm_get_parent_directory(const char *filepath);
char* fm_get_filename(const char *filepath);
int fm_is_absolute_path(const char *path);

#endif // FILE_MANAGER_H