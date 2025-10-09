#ifndef LOGGER_H
#define LOGGER_H

#include "file_operations.h"
#include <stdio.h>

#define MAX_LOG_MESSAGE 2048
#define MAX_LOG_FILE_SIZE 10485760

typedef struct {
    FILE* log_file;
    char log_path[MAX_PATH];
    LogLevel min_level;
    bool console_output;
    bool file_output;
    CRITICAL_SECTION lock;
    unsigned long long current_size;
    int rotation_count;
} Logger;

int init_logger(Logger* logger, const char* log_path, LogLevel level);
int cleanup_logger(Logger* logger);
int log_message(Logger* logger, LogLevel level, const char* format, ...);
int log_file_operation(Logger* logger, const char* operation, const char* path, bool success);
int log_statistics(Logger* logger, const CopyStatistics* stats);
int rotate_log_file(Logger* logger);
const char* get_log_level_string(LogLevel level);

#endif