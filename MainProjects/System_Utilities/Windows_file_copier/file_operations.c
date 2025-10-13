#include "file_operations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

bool file_matches_filter(const char* filename, const FileFilter* filter) {
    if (!filename || !filter) return false;
    
    const char* name = strrchr(filename, '\\');
    if (name) name++;
    else name = filename;
    
    const char* pattern = filter->pattern;
    const char* str = name;
    
    while (*pattern && *str) {
        if (*pattern == '*') {
            pattern++;
            if (!*pattern) return true;
            
            while (*str) {
                if (file_matches_filter(str, &(FileFilter){0})) {
                    FileFilter temp = *filter;
                    strcpy(temp.pattern, pattern);
                    if (file_matches_filter(str, &temp)) return true;
                }
                str++;
            }
            return false;
        } else if (*pattern == '?') {
            pattern++;
            str++;
        } else if (tolower(*pattern) == tolower(*str)) {
            pattern++;
            str++;
        } else {
            return false;
        }
    }
    
    while (*pattern == '*') pattern++;
    
    return !*pattern && !*str;
}

bool should_copy_file(const FileInfo* info, const CopyOptions* options) {
    if (!info || !options) return false;
    
    if (options->skip_hidden && (info->attributes & FILE_ATTRIBUTE_HIDDEN)) {
        return false;
    }
    
    if (options->skip_system && (info->attributes & FILE_ATTRIBUTE_SYSTEM)) {
        return false;
    }
    
    if (info->size < options->min_file_size || info->size > options->max_file_size) {
        return false;
    }
    
    if (CompareFileTime(&info->modified_time, &options->modified_after) < 0 && 
        options->modified_after.dwLowDateTime != 0) {
        return false;
    }
    
    if (CompareFileTime(&info->modified_time, &options->modified_before) > 0 && 
        options->modified_before.dwLowDateTime != 0) {
        return false;
    }
    
    bool include_file = options->filter_count == 0;
    
    for (int i = 0; i < options->filter_count; i++) {
        if (file_matches_filter(info->path, &options->filters[i])) {
            if (options->filters[i].type == FILTER_INCLUDE) {
                include_file = true;
            } else {
                return false;
            }
        }
    }
    
    return include_file;
}

int create_directory_structure(const char* path) {
    if (!path) return -1;
    
    char temp[MAX_PATH_LENGTH];
    strncpy(temp, path, MAX_PATH_LENGTH - 1);
    temp[MAX_PATH_LENGTH - 1] = '\0';
    
    for (char* p = temp; *p; p++) {
        if (*p == '\\' || *p == '/') {
            char orig = *p;
            *p = '\0';
            
            if (strlen(temp) > 0 && temp[strlen(temp) - 1] != ':') {
                DWORD attr = GetFileAttributesA(temp);
                if (attr == INVALID_FILE_ATTRIBUTES) {
                    if (!CreateDirectoryA(temp, NULL)) {
                        DWORD error = GetLastError();
                        if (error != ERROR_ALREADY_EXISTS) {
                            return -1;
                        }
                    }
                }
            }
            *p = orig;
        }
    }
    
    DWORD attr = GetFileAttributesA(temp);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectoryA(temp, NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                return -1;
            }
        }
    }
    
    return 0;
}

int calculate_file_hash(const char* path, unsigned char* hash, size_t hash_size) {
    if (!path || !hash || hash_size < 32) return -1;
    
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return -1;
    
    memset(hash, 0, hash_size);
    
    unsigned long long checksum = 0;
    DWORD bytesRead;
    unsigned char buffer[8192];
    
    while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        for (DWORD i = 0; i < bytesRead; i++) {
            checksum = ((checksum << 5) + checksum) + buffer[i];
        }
    }
    
    memcpy(hash, &checksum, sizeof(checksum));
    
    CloseHandle(hFile);
    return 0;
}

int verify_file_copy(const char* source, const char* dest) {
    if (!source || !dest) return -1;
    
    FileInfo src_info, dst_info;
    if (get_file_info(source, &src_info) != 0) return -1;
    if (get_file_info(dest, &dst_info) != 0) return -1;
    
    if (src_info.size != dst_info.size) return -1;
    
    unsigned char src_hash[32], dst_hash[32];
    if (calculate_file_hash(source, src_hash, 32) != 0) return -1;
    if (calculate_file_hash(dest, dst_hash, 32) != 0) return -1;
    
    if (memcmp(src_hash, dst_hash, 32) != 0) return -1;
    
    return 0;
}

int copy_single_file(const char* source, const char* dest, const CopyOptions* options) {
    if (!source || !dest || !options) return -1;
    
    HANDLE hSource = CreateFileA(source, GENERIC_READ, FILE_SHARE_READ, NULL,
                                 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hSource == INVALID_HANDLE_VALUE) return -1;
    
    char dest_dir[MAX_PATH_LENGTH];
    strncpy(dest_dir, dest, MAX_PATH_LENGTH - 1);
    dest_dir[MAX_PATH_LENGTH - 1] = '\0';
    
    char* last_slash = strrchr(dest_dir, '\\');
    if (last_slash) {
        *last_slash = '\0';
        if (create_directory_structure(dest_dir) != 0) {
            CloseHandle(hSource);
            return -1;
        }
    }
    
    HANDLE hDest = CreateFileA(dest, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDest == INVALID_HANDLE_VALUE) {
        CloseHandle(hSource);
        return -1;
    }
    
    DWORD bytesRead, bytesWritten;
    int result = 0;
    
    while (ReadFile(hSource, copy_buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        if (!WriteFile(hDest, copy_buffer, bytesRead, &bytesWritten, NULL) || 
            bytesWritten != bytesRead) {
            result = -1;
            break;
        }
    }
    
    if (options->preserve_attributes && result == 0) {
        BY_HANDLE_FILE_INFORMATION fileInfo;
        if (GetFileInformationByHandle(hSource, &fileInfo)) {
            SetFileTime(hDest, &fileInfo.ftCreationTime, 
                       &fileInfo.ftLastAccessTime, &fileInfo.ftLastWriteTime);
        }
    }
    
    CloseHandle(hSource);
    CloseHandle(hDest);
    
    if (result == 0 && options->preserve_attributes) {
        DWORD attrs = GetFileAttributesA(source);
        if (attrs != INVALID_FILE_ATTRIBUTES) {
            SetFileAttributesA(dest, attrs);
        }
    }
    
    if (result == 0 && options->verify_copy) {
        result = verify_file_copy(source, dest);
    }
    
    return result;
}

// Helper function to count files and bytes before copying
static void count_files_recursive(const char* source, const CopyOptions* options, CopyStatistics* stats) {
    if (!source || !options || !stats) return;
    
    char search_path[MAX_PATH_LENGTH];
    snprintf(search_path, MAX_PATH_LENGTH, "%s\\*", source);
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        if (strcmp(findData.cFileName, ".") == 0 || 
            strcmp(findData.cFileName, "..") == 0) {
            continue;
        }
        
        char src_path[MAX_PATH_LENGTH];
        snprintf(src_path, MAX_PATH_LENGTH, "%s\\%s", source, findData.cFileName);
        
        FileInfo info;
        if (get_file_info(src_path, &info) != 0) {
            continue;
        }
        
        if (info.is_directory) {
            if (options->recursive) {
                count_files_recursive(src_path, options, stats);
            }
        } else {
            if (should_copy_file(&info, options)) {
                stats->total_files++;
                stats->total_bytes += info.size;
            }
        }
        
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
}

int copy_directory_recursive(const char* source, const char* dest, 
                            const CopyOptions* options, CopyStatistics* stats) {
    if (!source || !dest || !options || !stats) return -1;
    
    char search_path[MAX_PATH_LENGTH];
    snprintf(search_path, MAX_PATH_LENGTH, "%s\\*", source);
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return -1;
    
    create_directory_structure(dest);
    
    do {
        if (strcmp(findData.cFileName, ".") == 0 || 
            strcmp(findData.cFileName, "..") == 0) {
            continue;
        }
        
        char src_path[MAX_PATH_LENGTH];
        char dst_path[MAX_PATH_LENGTH];
        
        snprintf(src_path, MAX_PATH_LENGTH, "%s\\%s", source, findData.cFileName);
        snprintf(dst_path, MAX_PATH_LENGTH, "%s\\%s", dest, findData.cFileName);
        
        FileInfo info;
        if (get_file_info(src_path, &info) != 0) {
            stats->failed_files++;
            continue;
        }
        
        if (info.is_directory) {
            if (options->recursive) {
                copy_directory_recursive(src_path, dst_path, options, stats);
            }
        } else {
            if (should_copy_file(&info, options)) {
                if (copy_single_file(src_path, dst_path, options) == 0) {
                    stats->copied_files++;
                    stats->copied_bytes += info.size;
                    
                    // Call progress callback if provided
                    if (stats->progress_callback) {
                        stats->progress_callback(stats);
                    }
                } else {
                    stats->failed_files++;
                }
            } else {
                stats->skipped_files++;
            }
        }
        
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
    return 0;
}

int perform_copy_operation(const CopyOptions* options, CopyStatistics* stats) {
    if (!options || !stats) return -1;
    
    // Save the progress callback before memset
    void (*saved_callback)(const struct CopyStatistics*) = stats->progress_callback;
    
    memset(stats, 0, sizeof(CopyStatistics));
    stats->progress_callback = saved_callback;  // Restore callback
    stats->start_time = GetTickCount();
    stats->in_progress = true;
    
    FileInfo src_info;
    if (get_file_info(options->source, &src_info) != 0) {
        stats->in_progress = false;
        printf("\n  ERROR: Cannot access source path\n");
        return -1;
    }
    
    int result;
    if (src_info.is_directory) {
        // First, count all files and bytes
        printf("\n  Scanning files...\n");
        count_files_recursive(options->source, options, stats);
        printf("  Found %llu files (%.2f MB)\n\n", 
               stats->total_files, 
               stats->total_bytes / (1024.0 * 1024.0));
        
        // Reset copied counters (total_files and total_bytes are preserved)
        stats->copied_files = 0;
        stats->copied_bytes = 0;
        stats->failed_files = 0;
        stats->skipped_files = 0;
        
        // Now perform the actual copy
        result = copy_directory_recursive(options->source, options->destination, 
                                         options, stats);
    } else {
        stats->total_files = 1;
        stats->total_bytes = src_info.size;
        
        if (should_copy_file(&src_info, options)) {
            result = copy_single_file(options->source, options->destination, options);
            if (result == 0) {
                stats->copied_files = 1;
                stats->copied_bytes = src_info.size;
            } else {
                stats->failed_files = 1;
            }
        } else {
            stats->skipped_files = 1;
            result = 0;
        }
    }
    
    stats->end_time = GetTickCount();
    stats->in_progress = false;
    
    return result;
}