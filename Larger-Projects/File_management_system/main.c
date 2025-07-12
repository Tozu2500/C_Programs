#include "file_manager.h"

void print_menu() {
    printf("\n=== File Management System ===\n");
    printf("1. Create file\n");
    printf("2. Read file\n");
    printf("3. Write to file\n");
    printf("4. Delete file\n");
    printf("5. Copy file\n");
    printf("6. Move/Rename file\n");
    printf("7. Create directory\n");
    printf("8. Delete directory\n");
    printf("9. List directory\n");
    printf("10. Change directory\n");
    printf("11. Get current directory\n");
    printf("12. Get file info\n");
    printf("13. Search files\n");
    printf("14. Set file permissions\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

void handle_create_file() {
    char filepath[MAX_PATH_LENGTH];
    char content[BUFFER_SIZE];
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    printf("Enter content (or press Enter for empty file): ");
    getchar(); // consume newline
    fgets(content, BUFFER_SIZE, stdin);
    
    FileOpResult result = fm_create_file(filepath, content);
    if (result == FILE_OP_SUCCESS) {
        printf("File created successfully!\n");
    } else {
        printf("Error creating file: %s\n", fm_get_error_message(result));
    }
}

void handle_read_file() {
    char filepath[MAX_PATH_LENGTH];
    char *content;
    long size;
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    FileOpResult result = fm_read_file(filepath, &content, &size);
    if (result == FILE_OP_SUCCESS) {
        printf("File content (%ld bytes):\n", size);
        printf("%s\n", content);
        free(content);
    } else {
        printf("Error reading file: %s\n", fm_get_error_message(result));
    }
}

void handle_write_file() {
    char filepath[MAX_PATH_LENGTH];
    char content[BUFFER_SIZE];
    int append;
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    printf("Append to file? (1=yes, 0=no): ");
    scanf("%d", &append);
    
    printf("Enter content: ");
    getchar(); // consume newline
    fgets(content, BUFFER_SIZE, stdin);
    
    FileOpResult result = fm_write_file(filepath, content, append);
    if (result == FILE_OP_SUCCESS) {
        printf("File written successfully!\n");
    } else {
        printf("Error writing file: %s\n", fm_get_error_message(result));
    }
}

void handle_delete_file() {
    char filepath[MAX_PATH_LENGTH];
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    FileOpResult result = fm_delete_file(filepath);
    if (result == FILE_OP_SUCCESS) {
        printf("File deleted successfully!\n");
    } else {
        printf("Error deleting file: %s\n", fm_get_error_message(result));
    }
}

void handle_copy_file() {
    char source[MAX_PATH_LENGTH];
    char destination[MAX_PATH_LENGTH];
    
    printf("Enter source file path: ");
    scanf("%s", source);
    
    printf("Enter destination file path: ");
    scanf("%s", destination);
    
    FileOpResult result = fm_copy_file(source, destination);
    if (result == FILE_OP_SUCCESS) {
        printf("File copied successfully!\n");
    } else {
        printf("Error copying file: %s\n", fm_get_error_message(result));
    }
}

void handle_move_file() {
    char source[MAX_PATH_LENGTH];
    char destination[MAX_PATH_LENGTH];
    
    printf("Enter source file path: ");
    scanf("%s", source);
    
    printf("Enter destination file path: ");
    scanf("%s", destination);
    
    FileOpResult result = fm_move_file(source, destination);
    if (result == FILE_OP_SUCCESS) {
        printf("File moved successfully!\n");
    } else {
        printf("Error moving file: %s\n", fm_get_error_message(result));
    }
}

void handle_create_directory() {
    char dirpath[MAX_PATH_LENGTH];
    
    printf("Enter directory path: ");
    scanf("%s", dirpath);
    
    FileOpResult result = fm_create_directory(dirpath);
    if (result == FILE_OP_SUCCESS) {
        printf("Directory created successfully!\n");
    } else {
        printf("Error creating directory: %s\n", fm_get_error_message(result));
    }
}

void handle_delete_directory() {
    char dirpath[MAX_PATH_LENGTH];
    
    printf("Enter directory path: ");
    scanf("%s", dirpath);
    
    FileOpResult result = fm_delete_directory(dirpath);
    if (result == FILE_OP_SUCCESS) {
        printf("Directory deleted successfully!\n");
    } else {
        printf("Error deleting directory: %s\n", fm_get_error_message(result));
    }
}

void handle_list_directory() {
    char dirpath[MAX_PATH_LENGTH];
    DirectoryListing listing;
    
    printf("Enter directory path (or . for current): ");
    scanf("%s", dirpath);
    
    FileOpResult result = fm_list_directory(dirpath, &listing);
    if (result == FILE_OP_SUCCESS) {
        fm_print_directory_listing(&listing);
        fm_free_directory_listing(&listing);
    } else {
        printf("Error listing directory: %s\n", fm_get_error_message(result));
    }
}

void handle_change_directory() {
    char dirpath[MAX_PATH_LENGTH];
    
    printf("Enter directory path: ");
    scanf("%s", dirpath);
    
    FileOpResult result = fm_change_directory(dirpath);
    if (result == FILE_OP_SUCCESS) {
        printf("Directory changed successfully!\n");
    } else {
        printf("Error changing directory: %s\n", fm_get_error_message(result));
    }
}

void handle_get_current_directory() {
    char *cwd = fm_get_current_directory();
    if (cwd) {
        printf("Current directory: %s\n", cwd);
        free(cwd);
    } else {
        printf("Error getting current directory\n");
    }
}

void handle_get_file_info() {
    char filepath[MAX_PATH_LENGTH];
    FileInfo info;
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    FileOpResult result = fm_get_file_info(filepath, &info);
    if (result == FILE_OP_SUCCESS) {
        fm_print_file_info(&info);
    } else {
        printf("Error getting file info: %s\n", fm_get_error_message(result));
    }
}

void handle_search_files() {
    char directory[MAX_PATH_LENGTH];
    char pattern[MAX_FILENAME_LENGTH];
    DirectoryListing results;
    
    printf("Enter directory to search: ");
    scanf("%s", directory);
    
    printf("Enter search pattern: ");
    scanf("%s", pattern);
    
    FileOpResult result = fm_search_files(directory, pattern, &results);
    if (result == FILE_OP_SUCCESS) {
        printf("Search results:\n");
        fm_print_directory_listing(&results);
        fm_free_directory_listing(&results);
    } else {
        printf("Error searching files: %s\n", fm_get_error_message(result));
    }
}

void handle_set_permissions() {
    char filepath[MAX_PATH_LENGTH];
    int permissions;
    
    printf("Enter file path: ");
    scanf("%s", filepath);
    
    printf("Enter permissions (octal, e.g., 755): ");
    scanf("%o", &permissions);
    
    FileOpResult result = fm_set_permissions(filepath, permissions);
    if (result == FILE_OP_SUCCESS) {
        printf("Permissions set successfully!\n");
    } else {
        printf("Error setting permissions: %s\n", fm_get_error_message(result));
    }
}

int main() {
    int choice;
    
    printf("Welcome to the File Management System!\n");
    
    while (1) {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                handle_create_file();
                break;
            case 2:
                handle_read_file();
                break;
            case 3:
                handle_write_file();
                break;
            case 4:
                handle_delete_file();
                break;
            case 5:
                handle_copy_file();
                break;
            case 6:
                handle_move_file();
                break;
            case 7:
                handle_create_directory();
                break;
            case 8:
                handle_delete_directory();
                break;
            case 9:
                handle_list_directory();
                break;
            case 10:
                handle_change_directory();
                break;
            case 11:
                handle_get_current_directory();
                break;
            case 12:
                handle_get_file_info();
                break;
            case 13:
                handle_search_files();
                break;
            case 14:
                handle_set_permissions();
                break;
            case 0:
                printf("Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}