#include "file_manager.h"

void print_menu() {
    printf("\n=== File Management System ===\n");
    printf("1. Create a new file\n");
    printf("2. Read file\n");
    printf("3. Write to a file\n");
    printf("4. Delete a file\n");
    printf("5. Copy file\n");
    printf("6. Move or Rename a file\n");
    printf("7. Create new directory\n");
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
    char filepath[MAX_FILENAME_LENGTH];
    char content[BUFFER_SIZE];

    printf("Enter file path: ");
    scanf("%s", filepath);

    printf("Enter content (or press Enter for empty file): ");
    getchar();
    fgets(content, BUFFER_SIZE, stdin);

    FileOpResult result = fm_create_file(filepath, content);
    if (result == FILE_OP_SUCCESS) {
        printf("File was created successfully!\n");
    } else {
        printf("There was an error creating file: %s\n", fm_get_error_message(result));
    }
}

void handle_read_file() {
    char filepath[MAX_PATH_LENGTH];
    char *content;
    long size;

    printf("Enter filepath: ");
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

    printf("Enter filepath: ");
    scanf("%s", filepath);

    printf("Append to file? (1=Yes, 2=No): ");
    scanf("%d", &append);

    printf("Enter content: ");
    getchar();
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
        printf("File was deleted successfully!\n");
    } else {
        printf("There was an error deleting file: %s\n", fm_get_error_message(result));
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
        printf("File was moved successfully!\n");
    } else {
        printf("Error moving file: %s\n", fm_get_error_message(result));
    }
}

void handle_create_directory() {
    char dirpath[MAX_PATH_LENGTH];

    printf("Enter directory path: ");
}