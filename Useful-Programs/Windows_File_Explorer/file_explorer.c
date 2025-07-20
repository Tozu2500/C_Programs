#include "file_explorer.h"

// Initialization
void init_explorer(FileExplorer *explorer) {
    GetCurrentDirectory(MAX_PATH_LEN, explorer->current_path);
    explorer->file_count = 0;
    list_directory(explorer, explorer->current_path);
}

// List files and dirs in the specified path
int list_directory(FileExplorer *explorer, const char *path) {
    WIN32_FIND_DATA find_data;
    HANDLE find_handle;
    char search_path[MAX_PATH_LEN];

    // Clearing the previous file list
    explorer->file_count = 0;

    // Creating search pattern
    snprintf(search_path, MAX_PATH_LEN, "%s\\*", path);

    find_handle = FindFirstFile(search_path, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) {
        printf("Error, can't access directory '%s'\n", path);
        return 0;
    }

    do {
        // Skip the current directory entry
        if (strcmp(find_data.cFileName, ".") == 0) {
            continue;
        }

        // Add a file or a directory to the list
        if (explorer->file_count < MAX_FILES) {
            FileEntry *entry = &explorer->files[explorer->file_count];

            strncpy(entry->name, find_data.cFileName, MAX_PATH_LEN - 1);
            entry->name[MAX_PATH_LEN - 1] = '\0';

            snprintf(entry->full_path, MAX_PATH_LEN, "%s\\%s", path, find_data.cFileName);

            entry->attributes = find_data.dwFileAttributes;
            entry->file_size.LowPart = find_data.nFileSizeLow;
            entry->file_size.HighPart = find_data.nFileSizeHigh;
            entry->last_modified = find_data.ftLastWriteTime;
            entry->is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            explorer->file_count++;
        }
    } while (FindNextFile(find_handle, &find_data) != 0);

    FindClose(find_handle);
    strcpy(explorer->current_path, path);
    return 1;
}

// Display the current directory contents
void display_files(const FileExplorer *explorer) {
    printf("\n=== Current Directory: %s === \n", explorer->current_path);
    printf("%-4s %-30s %-10s %-20s %s\n", "ID", "Name", "Type", "Size" "Modified");
    printf("%-4s %-30s %-10s %-20s %s\n", "---", "----", "----", "----", "-------");

    for (int i = 0; i < explorer->file_count; i++) {
        const FileEntry *file = &explorer->files[i];
        char size_str[20];
        char time_str[30];

        format_file_size(file->file_size, size_str);
        format_file_size((FILETIME*)&file->last_modified, time_str);

        printf("%-4d %-30s %-10s %-20s %s\n",
                i + 1,
                file->name,
                file->is_directory ? "[DIR]" : "[FILE]",
                file->is_directory ? "" : size_str,
                time_str);
    }

    printf("\nTotal items: %d\n", explorer->file_count);
}

// Display detailed info about a file
void display_file_info(const FileEntry *file) {
    char size_str[20];
    char time_str[30];

    printf("\n--- File Information ---\n");
    printf("Name: %s\n", file->name);
    printf("Full path: %s\n", file->full_path);
    printf("Type: %s\n", file->is_directory ? "Directory" : "File");

    if (!file->is_directory) {
        format_file_size(file->file_size, size_str);
        printf("Size: %s\n", size_str);
    }

    format_file_time((FILETIME*)&file->last_modified, time_str);
    printf("Last modified: %s\n", time_str);

    printf("Attributes: ");
    if (file->attributes & FILE_ATTRIBUTE_READONLY) printf("Read-only ");
    if (file->attributes & FILE_ATTRIBUTE_HIDDEN) printf("Hidden ");
    if (file->attributes & FILE_ATTRIBUTE_SYSTEM) printf("System ");
    if (file->attributes & FILE_ATTRIBUTE_ARCHIVE) printf("Archive ");
    printf("\n");
}

// Change to a subdir
int change_directory(FileExplorer *explorer, const char *dir_name) {
    char new_path[MAX_PATH_LEN];

    // Handle special case for parent directory
    if (strcmp(dir_name, "..") == 0) {
        return go_parent_directory(explorer);
    }

    // Find the directory in the current listing
    for (int i = 0; i < explorer->file_count; i++) {
        if (explorer->files[i].is_directory && strcmp(explorer->files[i].name, dir_name) == 0) {
            strcpy(new_path, explorer->files[i].full_path);
            return list_directory(explorer, new_path);
        }
    }

    printf("Directory '%s' was not found in the current location\n", dir_name);
    return 0;
}

// Go to a parent dir
int go_parent_directory(FileExplorer *explorer) {
    char *last_slash = strrchr(explorer->current_path, '\\');

    if (last_slash != NULL && last_slash != explorer->current_path) {
        // Don't go above the root (C:\)
        if (last_slash == explorer->current_path + 2 && explorer->current_path[i] == ':') {
            printf("Already at root directory.\n");
            return 0;
        }

        *last_slash = '\0';
        return list_directory(explorer, explorer->current_path);
    }

    printf("Already at root directory.\n");
    return 0;
}

// Format file size for display
void format_file_size(LARGE_INTEGER size, char *buffer) {
    double size_d = (double)size.QuadPart;

    if (size_d >= 1073741824.0) {  // 1GB
        snprintf(buffer, 20, "%1.f GB", size_d / 1073741824.0);
    } else if (size_d >= 1048576.0) {  // 1MB
        snprintf(buffer, 20, "%.1f MB", size_d / 1048576.0);
    } else if (size_d >= 1024.0) {  // 1KB
        snprintf(buffer, 20, "%1.f KB", size_d / 1024.0);
    } else {
        snprintf(buffer, 20, "%.0f bytes", size_d);
    }
}

// Format file time for display
void format_file_time(FILETIME *ft, char *buffer) {
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);

    snprintf(buffer, 30, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
            st.wHour, st.Wminute, st.wSecond);
}

// Clear the console screen
void clear_screen(void) {
    system("cls");
}

// Show help info
void show_help(void) {
    printf("\n=== File Explorer Help ===\n");
    printf("Commands:\n");
    printf("  ls, dir        - List current directory contents\n");
    printf("  cd <dirname>   - Change to directory\n");
    printf("  cd ..          - Go to parent directory\n");
    printf("  info <id>      - Show detailed info for file/directorty by ID\n");
    printf("  cls, clear     - Clear the screen\n");
    printf("  help, ?        - Show this help message\n");
    printf("  quit, exit     - Exit the program\n");
    printf("\nNavigation:\n");
    printf("  - Use directory names or '..' to navigate\n");
    printf("  - File/directory IDs are shown in the first column\n");
    printf("  - Use 'info <id>' to get detailed information\n");
}

// Check if a path is valid
int is_valid_path(const char *path) {
    DWORD attrs = GetFileAttributes(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
}
