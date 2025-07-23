#include "file_utils.h"
#include "display.h"
#include "input_handler.h"
#include "security.h"
#include <stdio.h>
#include <stdlib.h>

int interactive_mode(void);
int process_single_file(const char* filepath, int verbose);

int main(int argc, char* argv[]) {
    CommandLineArgs args;

    if (!parse_command_line(argc, argv, &args)) {
        display_error("Failed to parse command line arguments");
        return 1;
    }

    display_banner();

    if (args.show_help) {
        display_help();
        return 0;
    }

    if (args.interactive_mode) {
        return interactive_mode();
    }

    if (strlen(args.filepath) == 0) {
        display_error("No file path specified");
        display_help();
        return 1;
    }

    return process_single_file(args.filepath, args.verbose_mode);
}

int interactive_mode(void) {
    char filepath[MAX_PATH];
    int continue_running = 1;

    printf("Interactive Mode\n");
    printf("================\n");
    printf("Enter file paths to analyze (type 'exit' to quit)\n\n");

    while (continue_running) {
        get_user_input(filepath, MAX_PATH, "Enter file path ");

        if (strcmp(filepath, "exit") == 0 || strcmp(filepath, "quit") == 0) {
            continue_running = 0;
            printf("Bye bye!\n");
            break;
        }

        if (strlen(filepath) == 0) {
            printf("Please enter a valid filepath.\n");
            continue;
        }

        printf("\n");
        process_single_file(filepath, 1);

        printf("\n");
        if (!confirm_action("Analyze another file?")) {
            continue_running = 0;
            printf("Bye bye!\n");
        } else {
            printf("\n");
        }
    }

    return 0;
}

int process_single_file(const char* filepath, int verbose) {
    char normalized_path[MAX_PATH];
    FileInfo file_info;
    SecurityInfo security_info;

    strcpy_s(normalized_path, MAX_PATH, filepath);
    normalize_path(normalized_path);

    if (!validate_file_path(normalized_path)) {
        display_error("Invalid path file format");
        return 1;
    }

    if (!file_exists(normalized_path)) {
        display_error("File or directory doesn't exist");
        return 1;
    }

    printf("Analyzing: %s\n", normalized_path);
    printf("======================================================\n");

    if (!get_file_info(normalized_path, &file_info)) {
        display_error("Failed to retrieve file information");
        return 1;
    }

    display_file_metadata(&file_info);
    display_size_analysis(&file_info);
    display_time_information(&file_info);
    display_attributes_table(&file_info);

    if (verbose) {
        if (is_file_safe_to_access(normalized_path)) {
            if (get_file_security_info(normalized_path, &security_info)) {
                display_security_info(&security_info);
                display_permission_summary(&security_info);
            } else {
                printf("Security Information: Not available\n\n");
            }
        } else {
            printf("Security Information: Skipped (system file)\n\n");
        }
    }

    printf("=====================================================\n");
    printf("Analysis complete.\n");

    return 0;
}