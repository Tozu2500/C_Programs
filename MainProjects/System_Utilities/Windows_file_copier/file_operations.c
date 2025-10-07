#include "file_operations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned char copy_buffer[BUFFER_SIZE];

int initialize_copy_options(CopyOptions* options) {
    if (!options) return -1;

    memset(options, 0, sizeof(CopyOptions));
    options->mode = COPY_MODE_SIMPLE;
    options->recursive = true;
    options->preserve_attributes = true;
    options->verify_copy = false;
    options->create_log = true;
    options->compress = false;
    options->compress = false;
    options->encrypt = false;
    options->thread_count = 1;
    options->filter_count = 0;
    options->log_level = LOG_LEVEL_INFO;
    options->follow_symlinks = false;
    options->skip_hidden = false;
    options->skip_system = false;
    options->max_file_size = 0xFFFFFFFFFFFFFFFFULL;
    options->min_file_size = 0;

    return 0;
}

int get_file_info(const char* path, FileInfo* info) {
    if (!path || !info) return -1;

    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &attr)) {
        return -1;
    }

    strncpy(info->path, path, MAX_PATH_LENGTH - 1);
    info->path[MAX_PATH_LENGTH - 1] = '\0';

    LARGE_INTEGER size;
    size.HighPart = attr.nFileSizeHigh;
    size.LowPart = attr.nFileSizeLow;
    info->size = size.QuadPart;

    info->modified_time = attr.ftLastWriteTime;
    info->attributes = attr.dwFileAttributes;
    info->is_directory = (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    return 0;
}