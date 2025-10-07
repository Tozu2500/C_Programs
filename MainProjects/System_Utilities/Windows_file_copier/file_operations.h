#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <windows.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 32768
#define BUFFER_SIZE 1048576
#define MAX_FILTERS 64
#define MAX_THREADS 16

typedef enum {
    COPY_MODE_SIMPLE,
    COPY_MODE_VERIFY,
    COPY_MODE_INCREMENTAL,
    COPY_MODE_DIFFERENTIAL,
    COPY_MODE_MIRROR
} CopyMode;

typedef enum {
    FILTER_INCLUDE,
    FILTER_EXCLUDE
} FilterType;

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} LogLevel;

typedef struct {
    char pattern[MAX_PATH];
    FilterType type;
    bool recursive;
} FileFilter;

typedef struct {
    char source[MAX_PATH_LENGTH];
    char destination[MAX_PATH_LENGTH];
    CopyMode mode;
    bool recursive;
    bool preserve_attributes;
    bool verify_copy;
    bool create_log;
    bool compress;
    bool encrypt;
    int thread_count;
    FileFilter filters[MAX_FILTERS];
    int filter_count;
    LogLevel log_level;
    bool follow_symlinks;
    bool skip_hidden;
    bool skip_system;
    unsigned long long max_file_size;
    unsigned long long min_file_size;
    FILETIME modified_after;
    FILETIME modified_before;
} CopyOptions;

typedef struct {
    unsigned long long total_files;
    unsigned long long copied_files;
    unsigned long long skipped_files;
    unsigned long long failed_files;
    unsigned long long total_bytes;
    unsigned long long copied_bytes;
    DWORD start_time;
    DWORD end_time;
    bool in_progress;
} CopyStatistics;

typedef struct {
    char path[MAX_PATH_LENGTH];
    unsigned long long size;
    FILETIME modified_time;
    DWORD attributes;
    bool is_directory;
} FileInfo;

int initialize_copy_options(CopyOptions* options);
int perform_copy_operation(const CopyOptions* options, CopyStatistics* stats);
int copy_single_file(const char* source, const char* dest, const CopyOptions* options);
int copy_directory_recursive(const char* source, const char* dest, const CopyOptions* options, CopyStatistics* stats);
bool should_copy_file(const FileInfo* info, const CopyOptions* options);
int verify_file_copy(const char* source, const char* dest);
int calculate_file_hash(const char* path, unsigned char* hash, size_t hash_size);
int create_directory_structure(const char* path);
bool file_matches_filter(const char* filename, const FileFilter* filter);
int get_file_info(const char* path, FileInfo* info);

#endif // FILE_OPERATIONS_H