#include "file_explorer.h"

int main(void) {
    FileExplorer explorer;
    char command[256];
    char arg[256];
    int file_id;

    printf("Windows File Explorer\n");
    printf("Type 'help' for available commands\n\n");

    // Initializing the explorer
    init_explorer(&explorer);
    display_files(&explorer);

    while (1) {
        printf("\n> ");
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        // Newline remove
        command[strcspn(command, "\n")] = '\0';

        // Empty commands are skipped
        if (strlen(command) == 0) {
            continue;
        }

        // Parsing the command and the argument
        if (sscanf(command, "%s %s", command, arg) < 1) {
            continue;
        }

        // Lowercasing
        for (int i = 0; command[i]; i++) {
            command[i] = tolower(command[i]);
        }

        // Handle commands
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            printf("Good bye!\n");
            break;
        } else if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0) {
            show_help();
        } else if (strcmp(command, "cls") == 0 || strcmp(command, "clear") == 0) {
            clear_screen();
            display_files(&explorer);
        } else if (strcmp(command, "ls") == 0 || strcmp(command, "dir") == 0) {
            display_files(&explorer);
        } else if (strcmp(command, "cd") == 0) {
            if (strlen(arg) > 0) {
                if (change_directory(&explorer, arg)) {
                    display_files(&explorer);
                }
            } else {
                printf("Usage: cd <directory_name>\n");
            }
        } else if (strcmp(command, "info") == 0) {
            if (strlen(arg) > 0) {
                file_id = atoi(arg);
                if (file_id >= 1 && file_id <= explorer.file_count) {
                    display_file_info(&explorer.files[file_id - 1]);
                } else {
                    printf("Invalid file ID. Use 1-%d\n", explorer.file_count);
                }
            } else {
                printf("Usage: info <file_id>\n");
            }
        } else {
            printf("Unknown command: %s\n", command);
            printf("Type 'help' for available commands\n");
        }

        // Clearing arg for next iteration
        arg[0] = '\0';
    }

    return 0;
}
