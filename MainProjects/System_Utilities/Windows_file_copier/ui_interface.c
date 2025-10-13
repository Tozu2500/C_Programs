#include "ui_interface.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define COLOR_RED 12
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_WHITE 15
#define COLOR_CYAN 11

static DWORD last_progress_time = 0;

int init_ui(UIConfig* config) {
    if (!config) return -1;

    config->mode = UI_MODE_INTERACTIVE;
    config->show_progress = true;
    config->colored_output = true;
    config->refresh_interval = 500;

    return 0;
}

int set_console_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    return 0;
}

int clear_screen(void) {
    system("cls");
    return 0;
}

int display_error(const char* message) {
    if (!message) return -1;

    set_console_color(COLOR_RED);
    printf("ERROR: %s\n", message);
    set_console_color(COLOR_WHITE);

    return 0;
}

int display_success(const char* message) {
    if (!message) return -1;

    set_console_color(COLOR_GREEN);
    printf("SUCCESS: %s\n", message);
    set_console_color(COLOR_WHITE);

    return 0;
}

int display_menu(void) {
    clear_screen();

    set_console_color(COLOR_CYAN);
    printf("\n");
    printf("  ===================================================\n");
    printf("  |   FILE COPIER & BACKUP TOOL                    |\n");
    printf("  ===================================================\n");
    set_console_color(COLOR_WHITE);
    
    printf("\n");
    printf("  [1] Copy Files/Folders\n");
    printf("  [2] Create Backup\n");
    printf("  [3] Restore Backup\n");
    printf("  [4] List Backups\n");
    printf("  [5] Verify Backup\n");
    printf("  [6] Delete Backup\n");
    printf("  [7] Configure Options\n");
    printf("  [8] Exit\n");
    printf("\n");

    return 0;
}

int get_user_choice(int min, int max) {
    int choice;
    char buffer[32];

    while (1) {
        printf("  Enter choice [%d-%d]: ", min, max);

        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &choice) == 1) {
                if (choice >= min && choice <= max) {
                    return choice;
                }
            }
        }

        display_error("Invalid choice, try again");
    }
}

int get_user_input(const char* prompt, char* buffer, size_t size) {
    if (!prompt || !buffer || size == 0) return -1;
    
    while (1) {
        printf("%s", prompt);
        
        if (!fgets(buffer, size, stdin)) {
            return -1;
        }
        
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        
        // Check if input is empty
        if (len == 0 || buffer[0] == '\0') {
            display_error("Input cannot be empty, please try again");
            continue;
        }
        
        // Trim leading whitespace
        size_t start = 0;
        while (buffer[start] == ' ' || buffer[start] == '\t') {
            start++;
        }
        
        // Check if input is only whitespace
        if (buffer[start] == '\0') {
            display_error("Input cannot be empty, please try again");
            continue;
        }
        
        // Move trimmed string to the beginning
        if (start > 0) {
            memmove(buffer, buffer + start, len - start + 1);
        }
        
        break;
    }
    
    return 0;
}

int confirm_action(const char* message) {
    if (!message) return 0;

    char response[10];
    
    while (1) {
        printf("%s (y/n): ", message);

        if (fgets(response, sizeof(response), stdin)) {
            // Remove newline
            size_t len = strlen(response);
            if (len > 0 && response[len - 1] == '\n') {
                response[len - 1] = '\0';
            }
            
            // Check for valid input
            if (response[0] == 'y' || response[0] == 'Y') {
                return 1;
            } else if (response[0] == 'n' || response[0] == 'N') {
                return 0;
            } else if (response[0] == '\0') {
                display_error("Please enter 'y' or 'n'");
                continue;
            } else {
                display_error("Invalid input. Please enter 'y' or 'n'");
                continue;
            }
        }
        
        return 0;
    }
}

int display_progress(const CopyStatistics* stats, bool force_refresh) {
    if (!stats) return -1;

    DWORD current_time = GetTickCount();

    if (!force_refresh && (current_time - last_progress_time) < 500) {
        return 0;
    }

    last_progress_time = current_time;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    COORD coord = {0, csbi.dwCursorPosition.Y};
    SetConsoleCursorPosition(hConsole, coord);

    double progress = 0.0;
    if (stats->total_bytes > 0) {
        progress = (double)stats->copied_bytes / (double)stats->total_bytes * 100.0;
    }

    printf("\r  Progress: [");

    int bar_width = 40;
    int filled = (int)(progress / 100.0 * bar_width);

    set_console_color(COLOR_GREEN);
    for (int i = 0; i < filled; i++) {
        printf("█");
    }
    set_console_color(COLOR_WHITE);

    for (int i = filled; i < bar_width; i++) {
        printf("░");
    }

    printf("] %.1f%%", progress);
    printf("(%llu/%llu files)", stats->copied_files, stats->total_files);
    printf("                    ");

    fflush(stdout);

    return 0;
}

int display_statistics(const CopyStatistics* stats) {
    if (!stats) return -1;

    printf("\n\n");
    set_console_color(COLOR_CYAN);
    printf("  ===================================================\n");
    printf("  |   OPERATION STATISTICS                          |\n");
    printf("  ===================================================\n");
    set_console_color(COLOR_WHITE);

    printf("\n");
    printf("  Total Files:    %llu\n", stats->total_files);
    printf("  Copied Files:   ");
    set_console_color(COLOR_GREEN);
    printf("%llu\n", stats->copied_files);
    set_console_color(COLOR_WHITE);
    
    printf("  Skipped Files:  ");
    set_console_color(COLOR_YELLOW);
    printf("%llu\n", stats->skipped_files);
    set_console_color(COLOR_WHITE);
    
    printf("  Failed Files:   ");
    if (stats->failed_files > 0) {
        set_console_color(COLOR_RED);
    }
    printf("%llu\n", stats->failed_files);
    set_console_color(COLOR_WHITE);
    
    printf("\n");
    printf("  Total Size:     %.2f MB\n", stats->total_bytes / 1024.0 / 1024.0);
    printf("  Copied Size:    %.2f MB\n", stats->copied_bytes / 1024.0 / 1024.0);
    
    DWORD duration = stats->end_time - stats->start_time;
    double duration_sec = duration / 1000.0;
    printf("  Duration:       %.2f seconds\n", duration_sec);
    
    if (duration_sec > 0) {
        double speed = stats->copied_bytes / duration_sec / 1024.0 / 1024.0;
        printf("  Average Speed:  %.2f MB/s\n", speed);
    }
    
    printf("\n");
    
    return 0;
}

int display_backup_list(const BackupCatalog* catalog) {
    if (!catalog) return -1;

    printf("\n");
    set_console_color(COLOR_CYAN);
    printf("  ===================================================\n");
    printf("  |   BACKUP CATALOG                                |\n");
    printf("  ===================================================\n");
    set_console_color(COLOR_WHITE);

    printf("\n  Total backups: %d\n\n", catalog->set_count);

    for (int i = 0; i < catalog->set_count; i++) {
        const BackupSet* set = &catalog->sets[i];

        set_console_color(COLOR_YELLOW);
        printf("  Backup #%d:\n", i + 1);
        set_console_color(COLOR_WHITE);

        printf("    Name:   %s\n", set->name);
        printf("    Type:   ");

        switch (set->type) {
            case BACKUP_TYPE_FULL:
                printf("Full\n");
                break;
            case BACKUP_TYPE_INCREMENTAL:
                printf("Incremental\n");
                break;
            case BACKUP_TYPE_DIFFERENTIAL:
                printf("Differential\n");
                break;
        }

        printf("    Source:   %s\n", set->source_path);
        printf("    Backup:   %s\n", set->backup_path);
        printf("    Files:    %llu\n", set->file_count);
        printf("    Size:     %.2f MB\n", set->total_size / 1024.0 / 1024.0);
        printf("    Valid:    %s\n", set->is_valid ? "Yes" : "No");
        printf("\n");
    }
    
    return 0;
}

int configure_copy_options_interactive(CopyOptions* options) {
    if (!options) return -1;
    
    int choice;
    
    printf("\n");
    set_console_color(COLOR_CYAN);
    printf("  ===================================================\n");
    printf("  |   CONFIGURE COPY OPTIONS                        |\n");
    printf("  ===================================================\n");
    set_console_color(COLOR_WHITE);
    printf("\n");
    
    printf("  Copy Mode:\n");
    printf("    [1] Simple Copy\n");
    printf("    [2] Verify Copy\n");
    printf("    [3] Incremental Copy\n");
    printf("    [4] Differential Copy\n");
    printf("    [5] Mirror Copy\n");
    choice = get_user_choice(1, 5);
    options->mode = (CopyMode)(choice - 1);
    
    printf("\n  Recursive copy? (1=Yes, 0=No): ");
    scanf("%d", &choice);
    getchar();
    options->recursive = (choice == 1);
    
    printf("  Preserve attributes? (1=Yes, 0=No): ");
    scanf("%d", &choice);
    getchar();
    options->preserve_attributes = (choice == 1);
    
    printf("  Verify after copy? (1=Yes, 0=No): ");
    scanf("%d", &choice);
    getchar();
    options->verify_copy = (choice == 1);
    
    printf("  Skip hidden files? (1=Yes, 0=No): ");
    scanf("%d", &choice);
    getchar();
    options->skip_hidden = (choice == 1);
    
    printf("  Skip system files? (1=Yes, 0=No): ");
    scanf("%d", &choice);
    getchar();
    options->skip_system = (choice == 1);
    
    printf("  Number of threads (1-%d): ", MAX_THREADS);
    scanf("%d", &choice);
    getchar();
    if (choice >= 1 && choice <= MAX_THREADS) {
        options->thread_count = choice;
    }
    
    printf("\n");
    display_success("Options configured successfully");
    
    return 0;
}