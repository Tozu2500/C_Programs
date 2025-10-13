#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

const char* get_log_level_string(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_WARNING: return "WARNING";
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int init_logger(Logger* logger, const char* log_path, LogLevel level) {
    if (!logger) return -1;
    
    memset(logger, 0, sizeof(Logger));
    
    if (log_path) {
        strncpy(logger->log_path, log_path, MAX_PATH - 1);
        logger->log_path[MAX_PATH - 1] = '\0';
        
        logger->log_file = fopen(log_path, "a");
        if (!logger->log_file) {
            return -1;
        }
        logger->file_output = true;
        
        fseek(logger->log_file, 0, SEEK_END);
        logger->current_size = ftell(logger->log_file);
    }
    
    logger->min_level = level;
    logger->console_output = true;
    logger->rotation_count = 0;
    
    InitializeCriticalSection(&logger->lock);
    
    return 0;
}

int cleanup_logger(Logger* logger) {
    if (!logger) return -1;
    
    if (logger->log_file) {
        fclose(logger->log_file);
        logger->log_file = NULL;
    }
    
    DeleteCriticalSection(&logger->lock);
    
    return 0;
}

int rotate_log_file(Logger* logger) {
    if (!logger || !logger->log_file) return -1;
    
    fclose(logger->log_file);
    
    char backup_path[MAX_PATH];
    snprintf(backup_path, MAX_PATH, "%s.%d", logger->log_path, logger->rotation_count);
    
    if (!MoveFileA(logger->log_path, backup_path)) {
        DeleteFileA(backup_path);
        MoveFileA(logger->log_path, backup_path);
    }
    
    logger->log_file = fopen(logger->log_path, "a");
    if (!logger->log_file) return -1;
    
    logger->current_size = 0;
    logger->rotation_count++;
    
    return 0;
}

int log_message(Logger* logger, LogLevel level, const char* format, ...) {
    if (!logger || !format) return -1;
    
    if (level < logger->min_level) return 0;
    
    EnterCriticalSection(&logger->lock);
    
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    char message[MAX_LOG_MESSAGE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, MAX_LOG_MESSAGE, format, args);
    va_end(args);
    
    char log_line[MAX_LOG_MESSAGE + 128];
    snprintf(log_line, sizeof(log_line), "[%s] [%s] %s\n", 
             time_str, get_log_level_string(level), message);
    
    if (logger->console_output) {
        printf("%s", log_line);
    }
    
    if (logger->file_output && logger->log_file) {
        fprintf(logger->log_file, "%s", log_line);
        fflush(logger->log_file);
        
        logger->current_size += strlen(log_line);
        
        if (logger->current_size >= MAX_LOG_FILE_SIZE) {
            rotate_log_file(logger);
        }
    }
    
    LeaveCriticalSection(&logger->lock);
    
    return 0;
}

int log_file_operation(Logger* logger, const char* operation, const char* path, bool success) {
    if (!logger || !operation || !path) return -1;
    
    LogLevel level = success ? LOG_LEVEL_INFO : LOG_LEVEL_ERROR;
    const char* status = success ? "SUCCESS" : "FAILED";
    
    return log_message(logger, level, "%s: %s - %s", operation, path, status);
}

int log_statistics(Logger* logger, const CopyStatistics* stats) {
    if (!logger || !stats) return -1;
    
    DWORD duration = stats->end_time - stats->start_time;
    double duration_sec = duration / 1000.0;
    
    log_message(logger, LOG_LEVEL_INFO, "=== Operation Statistics ===");
    log_message(logger, LOG_LEVEL_INFO, "Total files: %llu", stats->total_files);
    log_message(logger, LOG_LEVEL_INFO, "Copied files: %llu", stats->copied_files);
    log_message(logger, LOG_LEVEL_INFO, "Skipped files: %llu", stats->skipped_files);
    log_message(logger, LOG_LEVEL_INFO, "Failed files: %llu", stats->failed_files);
    log_message(logger, LOG_LEVEL_INFO, "Total bytes: %llu", stats->total_bytes);
    log_message(logger, LOG_LEVEL_INFO, "Copied bytes: %llu", stats->copied_bytes);
    log_message(logger, LOG_LEVEL_INFO, "Duration: %.2f seconds", duration_sec);
    
    if (duration_sec > 0) {
        double speed = stats->copied_bytes / duration_sec / 1024.0 / 1024.0;
        log_message(logger, LOG_LEVEL_INFO, "Average speed: %.2f MB/s", speed);
    }
    
    return 0;
}