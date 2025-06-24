#ifndef BACKUP_H
#define BACKUP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_PATH_LENGTH 4096 // Maximum length of a file path
#define MAX_FILENAME_LENGTH 256 // Maximum length of a file name
#define MAX_STRATEGIES 10 // Maximum number of backup strategies
#define BUFFER_SIZE 8192 // Size of the buffer for reading files
#define VERSIOR_PREFIX "v" // Prefix for version files
#define TIMESTAMP_FORMAT "%Y%m%d_%H%M%S" // Format for timestamps in version files

// Error codes
typedef enum {
    BACKUP_SUCCESS = 0,
    BACKUP_ERROR_INVALID_ARGS = 1,
    BACKUP_ERROR_FILE_NOT_FOUND = 2,
    BACKUP_ERROR_PERMISSION_DENIED = 3,
    BACKUP_ERROR_DISK_FULL = 4,
    BACKUP_ERROR_MEMORY = 5,
    BACKUP_ERROR_STRATEGY_NOT_FOUND = 6,
    BACKUP_ERROR_GENERAL = 7
} backup_error_t;

// Backup strategy structure
typedef enum {
    STRATEGY_SIMPLE = 0,
    STRATEGY_TIMESTAMPED = 1,
    STRATEGY_VERSIONED = 2,
    STRATEGY_INCREMENTAL = 3,
    STRATEGY_DIFFERENTIAL = 4,
    STRATEGY_COMPRESSED = 5,
} backup_strategy_t;

// Backup configuration structure
typedef struct {
    char source_path[MAX_PATH_LENGTH];
    char destination_path[MAX_PATH_LENGTH];
    backup_strategy_t strategy;
    int verbose;
    int dry_run;
    int recursive;
    int preserve_permissions;
    int compress_level;
    char exclude_patterns[10][MAX_FILENAME_LENGTH];
    int exclude_count;
} backup_config_t;

// File info structure
typedef struct {
    char path[MAX_PATH_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    size_t size;
    time_t modified_time;
    mode_t permissions;
    int is_directory;
} file_info_t;

// Backup statistics
typedef struct {
    int files_processed;
    int files_copied;
    int files_skipped;
    int directories_created;
    size_t total_bytes;
    time_t start_time;
    time_t end_time;
} backup_stats_t;

// Strategy function pointer type
typedef backup_error_t (*backup_strategy_func_t)(const backup_config_t* config, backup_stats_t* stats);

// Strategy registration structure
typedef struct {
    backup_strategy_t strategy;
    const char* name;
    const char* description;
    backup_strategy_func_t execute;
} strategy_registry_t;

// Function prototypes

// Core backup functions
backup_error_t backup_initialize(backup_config_t* config);
backup_error_t backup_execute(const backup_config_t* config, backup_stats_t* stats);
void backup_cleanup(backup_config_t* config);
void backup_print_stats(const backup_stats_t* stats);

// Strategy functions
backup_error_t strategy_simple(const backup_config_t* config, backup_stats_t* stats);
backup_error_t strategy_timestamped(const backup_config_t* config, backup_stats_t* stats);
backup_error_t strategy_versioned(const backup_config_t* config, backup_stats_t* stats);
backup_error_t strategy_incremental(const backup_config_t* config, backup_stats_t* stats);
backup_error_t strategy_differential(const backup_config_t* config, backup_stats_t* stats);
backup_error_t strategy_compressed(const backup_config_t* config, backup_stats_t* stats);

// Utility functions
backup_error_t create_directory_recursive(const char* path);
backup_error_t copy_file(const char* source, const char* destination, int preserve_permissions);
backup_error_t copy_directory_recursive(const char* source, const char* destination, const backup_config_t* config,
                                       backup_stats_t* stats);
backup_error_t copy_directory_incremental(const char* source, const char* destination,
                                        const backup_config_t* config, backup_stats_t* stats);
                                    
int should_exclude_file(const char* filename, const backup_config_t* config);
char* generate_timestamp_string(void);
int get_next_version_number(const char* base_path, const char* filename);

backup_error_t get_file_info(const char* path, file_info_t* info);

int file_needs_backup(const char* source_file, const char* dest_file);

// Configuration functions
backup_error_t parse_command_line(int argc, char* argv[], backup_config_t* config);
void print_usage(const char* program_name);
void print_version(void);

// Error and logging functions
const char* backup_error_string(backup_error_t error);
void log_error(const char* format, ...);
void log_info(const char* format, ...);
void log_verbose(const backup_config_t* config, const char* format, ...);

// Strategy registration functions
void register_strategies(void);
strategy_registry_t* find_strategy(backup_strategy_t strategy);
void list_strategies(void);

#endif // BACKUP_H