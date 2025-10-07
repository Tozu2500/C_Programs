#ifndef UI_H
#define UI_H

#include "sysmon.h"
#include "system_info.h"
#include "process_info.h"

typedef enum {
    VIEW_OVERVIEW,
    VIEW_PROCESSES,
    VIEW_MEMORY,
    VIEW_DETAILED_PROCESSES,
    VIEW_COUNT
} ViewType;

typedef struct {
    HANDLE console_handle;
    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    CONSOLE_CURSOR_INFO cursor_info;
    COORD original_cursor_pos;
    WORD original_attributes;
    int screen_width;
    int screen_height;
} ConsoleState;

typedef struct {
    int current_view;
    int process_sort_criteria;
    int show_system_processes;
    int auto_refresh;
    int refersh_interval;
    int selected_process_index;
    int process_list_offset;
    int show_detailed_info;
} UIState;

void init_console();
void cleanup_console();
void clear_screen();
void set_cursor_position(int x, int y);
void set_text_color(WORD color);
void reset_text_color();
void save_cursor_position();
void restore_cursor_position();
void hide_cursor();
void show_cursor();

void display_system_overview(SystemInfo* info);
void display_process_list(ProcessInfo* info, UIState* ui_state);
void display_memory_info(SystemInfo* info);
void display_detailed_process_info(ProcessInfo* info, UIState* ui_state);
void display_menu(UIState* ui_state);
void display_header(const char* title);
void display_status_bar(UIState* ui_state);
void display_help();

void draw_progress_bar(int x, int y, int width, double percent, WORD color);
void draw_box(int x, int y, int width, int height);
void draw_horizontal_line(int x, int y, int width);
void draw_vertical_line(int x, int y, int height);

void format_bytes(DWORDLONG bytes, char* buffer, int buffer_size);
void format_uptime(DWORD seconds, char* buffer, int buffer_size);
void format_time(FILETIME* ft, char* buffer, int buffer_size);
void format_percentage(double percent, char* buffer, int buffer_size);
void format_number(DWORD number, char* buffer, int buffer_size);

int handle_input(UIState* ui_state);
void handle_process_navigation(UIState* ui_state, char key);
void toggle_sort_criteria(UIState* ui_state);
void toggle_system_processes(UIState* ui_state);

extern ConsoleState g_console_state;

#define COLOR_DEFAULT 7
#define COLOR_HEADER 11
#define UI_COLOR_HIGHLIGHT 13
#define COLOR_WARNING 12
#define COLOR_SUCCESS 10
#define COLOR_PROGRESS_FILL 2
#define COLOR_PROGRESS_EMPTY 8

#endif // UI_H