#include "file_manager.h"

// Core file operations
FileOpResult fm_create_file(const char *filepath, const char *content) {
    if (!filepath) return FILE_OP_INVALID_PATH;
    
    FILE *file = fopen(filepath, "w");
    if (!file) {
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    if (content) {
        if (fputs(content, file) == EOF) {
            fclose(file);
            return FILE_OP_ERROR;
        }
    }
    
    fclose(file);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_read_file(const char *filepath, char **content, long *size) {
    if (!filepath || !content) return FILE_OP_INVALID_PATH;
    
    FILE *file = fopen(filepath, "r");
    if (!file) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    *content = malloc(file_size + 1);
    if (!*content) {
        fclose(file);
        return FILE_OP_ERROR;
    }
    
    size_t bytes_read = fread(*content, 1, file_size, file);
    (*content)[bytes_read] = '\0';
    
    if (size) *size = bytes_read;
    
    fclose(file);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_write_file(const char *filepath, const char *content, int append) {
    if (!filepath || !content) return FILE_OP_INVALID_PATH;
    
    FILE *file = fopen(filepath, append ? "a" : "w");
    if (!file) {
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    if (fputs(content, file) == EOF) {
        fclose(file);
        return FILE_OP_ERROR;
    }
    
    fclose(file);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_delete_file(const char *filepath) {
    if (!filepath) return FILE_OP_INVALID_PATH;
    
    if (remove(filepath) != 0) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    return FILE_OP_SUCCESS;
}

FileOpResult fm_copy_file(const char *source, const char *destination) {
    if (!source || !destination) return FILE_OP_INVALID_PATH;
    
    FILE *src = fopen(source, "rb");
    if (!src) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        return FILE_OP_ERROR;
    }
    
    FILE *dst = fopen(destination, "wb");
    if (!dst) {
        fclose(src);
        return FILE_OP_ERROR;
    }
    
    char buffer[BUFFER_SIZE];
    size_t bytes;
    
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            return FILE_OP_ERROR;
        }
    }
    
    fclose(src);
    fclose(dst);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_move_file(const char *source, const char *destination) {
    if (!source || !destination) return FILE_OP_INVALID_PATH;
    
    if (rename(source, destination) != 0) {
        // If rename fails, try copy and delete
        FileOpResult result = fm_copy_file(source, destination);
        if (result == FILE_OP_SUCCESS) {
            fm_delete_file(source);
        }
        return result;
    }
    
    return FILE_OP_SUCCESS;
}

FileOpResult fm_rename_file(const char *old_path, const char *new_path) {
    return fm_move_file(old_path, new_path);
}

// Directory operations
FileOpResult fm_create_directory(const char *dirpath) {
    if (!dirpath) return FILE_OP_INVALID_PATH;
    
#ifdef _WIN32
    if (mkdir(dirpath) != 0) {
#else
    if (mkdir(dirpath, 0755) != 0) {
#endif
        if (errno == EEXIST) return FILE_OP_ALREADY_EXISTS;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    return FILE_OP_SUCCESS;
}

FileOpResult fm_delete_directory(const char *dirpath) {
    if (!dirpath) return FILE_OP_INVALID_PATH;
    
    if (rmdir(dirpath) != 0) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    return FILE_OP_SUCCESS;
}

FileOpResult fm_list_directory(const char *dirpath, DirectoryListing *listing) {
    if (!dirpath || !listing) return FILE_OP_INVALID_PATH;
    
    DIR *dir = opendir(dirpath);
    if (!dir) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    listing->files = malloc(sizeof(FileInfo) * 10);
    listing->count = 0;
    listing->capacity = 10;
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (listing->count >= listing->capacity) {
            listing->capacity *= 2;
            listing->files = realloc(listing->files, sizeof(FileInfo) * listing->capacity);
        }
        
        FileInfo *info = &listing->files[listing->count];
        strncpy(info->name, entry->d_name, MAX_FILENAME_LENGTH - 1);
        info->name[MAX_FILENAME_LENGTH - 1] = '\0';
        
        char full_path[MAX_PATH_LENGTH];
#ifdef _WIN32
        snprintf(full_path, MAX_PATH_LENGTH, "%s\\%s", dirpath, entry->d_name);
#else
        snprintf(full_path, MAX_PATH_LENGTH, "%s/%s", dirpath, entry->d_name);
#endif
        strncpy(info->path, full_path, MAX_PATH_LENGTH - 1);
        info->path[MAX_PATH_LENGTH - 1] = '\0';
        
        struct stat st;
        if (stat(full_path, &st) == 0) {
            info->size = st.st_size;
            info->modified_time = st.st_mtime;
            info->is_directory = S_ISDIR(st.st_mode);
            info->permissions = st.st_mode;
        }
        
        listing->count++;
    }
    
    closedir(dir);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_change_directory(const char *dirpath) {
    if (!dirpath) return FILE_OP_INVALID_PATH;
    
    if (chdir(dirpath) != 0) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    return FILE_OP_SUCCESS;
}

char* fm_get_current_directory(void) {
    char *cwd = malloc(MAX_PATH_LENGTH);
    if (!cwd) return NULL;
    
    if (getcwd(cwd, MAX_PATH_LENGTH) == NULL) {
        free(cwd);
        return NULL;
    }
    
    return cwd;
}

// File information and utilities
FileOpResult fm_get_file_info(const char *filepath, FileInfo *info) {
    if (!filepath || !info) return FILE_OP_INVALID_PATH;
    
    struct stat st;
    if (stat(filepath, &st) != 0) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        return FILE_OP_ERROR;
    }
    
    char *filename = fm_get_filename(filepath);
    strncpy(info->name, filename, MAX_FILENAME_LENGTH - 1);
    info->name[MAX_FILENAME_LENGTH - 1] = '\0';
    
    strncpy(info->path, filepath, MAX_PATH_LENGTH - 1);
    info->path[MAX_PATH_LENGTH - 1] = '\0';
    
    info->size = st.st_size;
    info->modified_time = st.st_mtime;
    info->is_directory = S_ISDIR(st.st_mode);
    info->permissions = st.st_mode;
    
    return FILE_OP_SUCCESS;
}

int fm_file_exists(const char *filepath) {
    if (!filepath) return 0;
    return access(filepath, F_OK) == 0;
}

int fm_is_directory(const char *filepath) {
    if (!filepath) return 0;
    
    struct stat st;
    if (stat(filepath, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

long fm_get_file_size(const char *filepath) {
    if (!filepath) return -1;
    
    struct stat st;
    if (stat(filepath, &st) != 0) return -1;
    return st.st_size;
}

FileOpResult fm_set_permissions(const char *filepath, mode_t permissions) {
    if (!filepath) return FILE_OP_INVALID_PATH;
    
    if (chmod(filepath, permissions) != 0) {
        if (errno == ENOENT) return FILE_OP_NOT_FOUND;
        if (errno == EACCES) return FILE_OP_PERMISSION_DENIED;
        return FILE_OP_ERROR;
    }
    
    return FILE_OP_SUCCESS;
}

// Search and filtering
FileOpResult fm_search_files(const char *directory, const char *pattern, DirectoryListing *results) {
    if (!directory || !pattern || !results) return FILE_OP_INVALID_PATH;
    
    DirectoryListing listing;
    FileOpResult result = fm_list_directory(directory, &listing);
    if (result != FILE_OP_SUCCESS) return result;
    
    results->files = malloc(sizeof(FileInfo) * listing.count);
    results->count = 0;
    results->capacity = listing.count;
    
    for (int i = 0; i < listing.count; i++) {
        if (strstr(listing.files[i].name, pattern) != NULL) {
            results->files[results->count] = listing.files[i];
            results->count++;
        }
    }
    
    fm_free_directory_listing(&listing);
    return FILE_OP_SUCCESS;
}

FileOpResult fm_filter_by_extension(const DirectoryListing *input, const char *extension, DirectoryListing *output) {
    if (!input || !extension || !output) return FILE_OP_INVALID_PATH;
    
    output->files = malloc(sizeof(FileInfo) * input->count);
    output->count = 0;
    output->capacity = input->count;
    
    for (int i = 0; i < input->count; i++) {
        char *ext = fm_get_file_extension(input->files[i].name);
        if (ext && strcmp(ext, extension) == 0) {
            output->files[output->count] = input->files[i];
            output->count++;
        }
        free(ext);
    }
    
    return FILE_OP_SUCCESS;
}

// Utility functions
void fm_free_directory_listing(DirectoryListing *listing) {
    if (listing && listing->files) {
        free(listing->files);
        listing->files = NULL;
        listing->count = 0;
        listing->capacity = 0;
    }
}

const char* fm_get_error_message(FileOpResult result) {
    switch (result) {
        case FILE_OP_SUCCESS: return "Success";
        case FILE_OP_ERROR: return "General error";
        case FILE_OP_NOT_FOUND: return "File or directory not found";
        case FILE_OP_PERMISSION_DENIED: return "Permission denied";
        case FILE_OP_ALREADY_EXISTS: return "File or directory already exists";
        case FILE_OP_INVALID_PATH: return "Invalid path";
        default: return "Unknown error";
    }
}

char* fm_get_file_extension(const char *filename) {
    if (!filename) return NULL;
    
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    
    return strdup(dot + 1);
}

void fm_print_file_info(const FileInfo *info) {
    if (!info) return;
    
    printf("Name: %s\n", info->name);
    printf("Path: %s\n", info->path);
    printf("Size: %ld bytes\n", info->size);
    printf("Type: %s\n", info->is_directory ? "Directory" : "File");
    printf("Modified: %s", ctime(&info->modified_time));
    printf("Permissions: %o\n", info->permissions & 0777);
}

void fm_print_directory_listing(const DirectoryListing *listing) {
    if (!listing) return;
    
    printf("Directory contains %d items:\n", listing->count);
    for (int i = 0; i < listing->count; i++) {
        printf("%s %s (%ld bytes)\n", 
               listing->files[i].is_directory ? "[DIR]" : "[FILE]",
               listing->files[i].name,
               listing->files[i].size);
    }
}

// Path manipulation
char* fm_join_path(const char *dir, const char *filename) {
    if (!dir || !filename) return NULL;
    
    char *path = malloc(strlen(dir) + strlen(filename) + 2);
    if (!path) return NULL;
    
    strcpy(path, dir);
#ifdef _WIN32
    if (path[strlen(path) - 1] != '\\' && path[strlen(path) - 1] != '/') {
        strcat(path, "\\");
    }
#else
    if (path[strlen(path) - 1] != '/') {
        strcat(path, "/");
    }
#endif
    strcat(path, filename);
    
    return path;
}

char* fm_get_parent_directory(const char *filepath) {
    if (!filepath) return NULL;
    
    char *path = strdup(filepath);
#ifdef _WIN32
    char *last_slash = strrchr(path, '\\');
    if (!last_slash) last_slash = strrchr(path, '/');
#else
    char *last_slash = strrchr(path, '/');
#endif
    
    if (last_slash) {
        *last_slash = '\0';
    } else {
        strcpy(path, ".");
    }
    
    return path;
}

char* fm_get_filename(const char *filepath) {
    if (!filepath) return NULL;
    
#ifdef _WIN32
    const char *last_slash = strrchr(filepath, '\\');
    if (!last_slash) last_slash = strrchr(filepath, '/');
#else
    const char *last_slash = strrchr(filepath, '/');
#endif
    
    if (last_slash) {
        return strdup(last_slash + 1);
    }
    
    return strdup(filepath);
}

int fm_is_absolute_path(const char *path) {
    if (!path) return 0;
#ifdef _WIN32
    // Windows absolute paths: C:\path or \\server\share
    return (path[0] != '\0' && path[1] == ':') || (path[0] == '\\' && path[1] == '\\');
#else
    return path[0] == '/';
#endif
}