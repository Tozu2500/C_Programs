#include "input_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_command_line(int argc, char* argv[], CommandLineArgs* args) {
    memset(args, 0, sizeof(CommandLineArgs));

    if (argc < 2) {
        args->show_help = 1;
        return;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            args->show_help = 1;
        } else if (strcmp(argv[i], "-v") || strcmp(argv[i], "--verbose") == 0) {
            args->verbose_mode = 1;
        } else if (strcmp(argv[i], "-i") || strcmp(argv[i], "--interactive") == 0) {
            args->interactive_mode = 1;
        } else {
            strcpy_s(args->filepath, MAX_PATH, argv[i]);
        }
    }

    return 1;
}

void normalize_path(char* path) {
    int len = (int)strlen(path);

    for (int i = 0; i < len; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }

    if (len > 1 && path[len - 1] == '\\' && path[len - 2] != ':') {
        path[len - 1] = '\0';
    }

    if (strcmp(path, ".") == 0) {
        GetCurrentDirectoryA(MAX_PATH, path);
    }
}

int validate_file_path(const char* path) {
    if (!path || strlen(path) == 0) {
        return 0;
    }

    if (strlen(path) >= MAX_PATH) {
        return 0;
    }

    const char* invalid_chars = "<>:\"|?*";
    for (int i = 0; path[i] != '\0'; i++) {
        if (strchr(invalid_chars, path[i])) {
            return 0;
        }
    }

    return 1;
}

void get_user_input(char* buffer, int buffer_size, const char* prompt) {
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, buffer_size, stdin) != NULL) {
        int len = (int)strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int confirm_action(const char* message) {
    char response[10];
    printf("%s (y/n): ", message);
    fflush(stdout);

    if (fgets(response, sizeof(response), stdin) != NULL) {
        return (response[0] == 'y' || response[0] == 'Y');
    }

    return 0;
}