#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <time.h>

#define LOG_BUFFER_SIZE 256
#define TIME_BUFFER_SIZE 64

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG,
} log_level_t;

void log_message(log_level_t level, const char* format, ...);
void get_current_time_string(char* buffer, size_t buffer_size);
char* url_decode(const char* encoded);
void url_encode(const char* input, char* output, size_t output_size);
int is_safe_path(const char* path);
void sanitize_path(char* path);
size_t get_file_size(const char* filepath);
int create_directory_recursive(const char* path);

#endif // UTILS_H