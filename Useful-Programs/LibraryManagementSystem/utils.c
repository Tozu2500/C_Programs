#include "utils.h"
#include <string.h>

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_integer_input(const char* prompt) {
    int value;
    printf("%s", prompt);

    while (scanf("%d", &value) != 1) {
        printf("Please enter a valid integer: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    return value;
}

void get_string_input(const char* prompt, char* buffer, int max_length) {
    printf("%s", prompt);

    if (fgets(buffer, max_length, stdin)) {
        // Remove trailing newline if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

void print_separator(void) {
    printf("----------------------------------------\n");
}

void print_header(const char* title) {
    printf("\n");
    print_separator();
    printf("  %s\n", title);
    print_separator();
}