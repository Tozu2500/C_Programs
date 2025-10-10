#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include "file_operations.h"
#include "backup_manager.h"

typedef enum {
    UI_MODE_INTERACTIVE,
    UI_MODE_COMMAND_LINE,
    UI_MODE_SILENT
} UIMode;

typedef struct {
    UIMode mode;
    bool show_progress;
    bool colored_output;
    int refresh_interval;
} UIConfig;

int init_ui(UIConfig* config);
int display_menu(void);
int get_user_choice(int min, int max);
int get_user_input(const char* prompt, char* buffer, size_t size);
int display_progress(const CopyStatistics* stats, bool force_refresh);
int display_statistics(const CopyStatistics* stats);
int display_backup_list(const BackupCatalog* catalog);
int configure_copy_options_interactive(CopyOptions* options);
int set_console_color(int color);
int clear_screen(void);
int display_error(const char* message);
int display_success(const char* message);
int confirm_action(const char* message);

#endif