#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <windows.h>

typedef struct {
    char filepath[MAX_PATH];
    int show_help;
    int verbose_mode;
    int interactive_mode;
} CommandLineArgs;

int parse_command_line(int argc, char* argv[], CommandLineArgs* args);
void normalize_path(char* path);
int validate_file_path(const char* path);
void get_user_input(char* buffer, int buffer_size, const char* prompt);
int confirm_action(const char* message);

#endif // INPUT_HANDLER_H