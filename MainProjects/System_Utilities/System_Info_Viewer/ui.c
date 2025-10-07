#include "ui.h"

ConsoleState g_console_state = {0};

void init_console() {
    g_console_state.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(g_console_state.console_handle, &g_console_state.screen_info);

    g_console_state.original_attributes = g_console_state.screen_info.wAttributes;
    g_console_state.original_cursor_pos = g_console_state.screen_info.dwCursorPosition;
    g_console_state.screen_width = g_console_state.screen_info.srWindow.Right -
                                    g_console_state.screen_info.srWindow.Left + 1;
    
    g_console_state.screen_height = g_console_state.screen_info.srWindow.Bottom -
                                    g_console_state.screen_info.srWindow.Top + 1;

    GetConsoleCursorInfo(g_console_state.console_handle, &g_console_state.cursor_info);
    hide_cursor();
}

void cleanup_console() {
    show_cursor();
    reset_text_color();
    SetConsoleCursorInfo(g_console_state.console_handle, &g_console_state.cursor_info);
}

void clear_screen() {
    system("cls");
}

void set_cursor_position(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(g_console_state.console_handle, coord);
}

void set_text_color(WORD color) {
    SetConsoleTextAttribute(g_console_state.console_handle, color);
}

void reset_text_color() {
    SetConsoleTextAttribute(g_console_state.console_handle, g_console_state.original_attributes);
}

void save_cursor_position() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(g_console_state.console_handle, &csbi);
    g_console_state.original_cursor_pos = csbi.dwCursorPosition;
}

void restore_cursor_position() {
    SetConsoleCursorPosition(g_console_state.console_handle, g_console_state.original_cursor_pos);
}

void hide_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(g_console_state.console_handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(g_console_state.console_handle, &cursorInfo);
}

void show_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(g_console_state.console_handle, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(g_console_state.console_handle, &cursorInfo);
}

void display_system_overview(SystemInfo* info) {
    char buffer[256];

    display_header("System Overview");

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Computer: ");
    reset_text_color();
    printf("%s\n", info->computer_name);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("User: ");
    reset_text_color();
    printf("%s\n", info->username);

    format_uptime(info->uptime_seconds, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Uptime: ");
    reset_text_color();
    printf("%s\n\n", buffer);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("CPU Usage: ");
    reset_text_color();
    printf("%.1f%%\n", info->cpu_usage);
    draw_progress_bar(0, 8, PROGRESS_BAR_WIDTH, info->cpu_usage, COLOR_PROGRESS_FILL);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("\nMemory usage: ");
    reset_text_color();
    printf("%.1f%%\n", info->memory_usage_percent);
    draw_progress_bar(0, 11, PROGRESS_BAR_WIDTH, info->memory_usage_percent, COLOR_PROGRESS_FILL);

    format_bytes(info->used_memory, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("\nUsed memory: ");
    reset_text_color();
    printf("%s", buffer);
    format_bytes(info->total_memory, buffer, sizeof(buffer));
    printf(" / %s\n", buffer);

    format_bytes(info->available_memory, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Available memory: ");
    reset_text_color();
    printf("%s\n\n", buffer);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Processes: ");
    reset_text_color();
    printf("%lu\n", info->process_count);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Threads: ");
    reset_text_color();
    printf("%lu\n", info->thread_count);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("CPU Cores: ");
    reset_text_color();
    printf("%lu\n", info->sys_info.dwNumberOfProcessors);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Page size: ");
    reset_text_color();
    printf("%lu bytes\n", info->sys_info.dwPageSize);
}

void display_process_list(ProcessInfo* info, UIState* ui_state) {
    char buffer[64];
    int display_count = 15;

    display_header("Process list");

    printf("Total processes: %d (Showing top %d)\n", info->total_processes,
            (info->count < display_count) ? info->count : display_count);

    printf("Sort: %s | System processes: %s\n\n",
            (ui_state->process_sort_criteria == SORT_BY_MEMORY) ? "Memory" :
            (ui_state->process_sort_criteria == SORT_BY_CPU) ? "CPU" :
            (ui_state->process_sort_criteria == SORT_BY_NAME) ? "Name" : "PID",
            ui_state->show_system_processes ? "Shown" : "Hidden");

    set_text_color(COLOR_HEADER);
    printf("%-8s %-25s %-12s %-8s %-8s\n", "PID", "Name", "Memory", "Threads", "Handles");
    printf("%-8s %-25s %-12s %-8s %-8s\n", "----", "-------------------------------", "-----", "-------", "--------------");
    reset_text_color();

    for (int i = 0; i < info->count && i < display_count; i++) {
        if (i == ui_state->selected_process_index) {
            set_text_color(COLOR_SUCCESS);
        }

        format_bytes(info->processes[i].memory_usage, buffer, sizeof(buffer));
        printf("%-8lu %-25s %-12s %-8lu %-8lu\n",
            info->processes[i].pid,
            info->processes[i].name,
            buffer,
            info->processes[i].thread_count,
            info->processes[i].handle_count);
            
        if (i == ui_state->selected_process_index) {
            reset_text_color();
        }
    }
}

void display_memory_info(SystemInfo* info) {
    char total_buf[64], used_buf[64], free_buf[64], virtual_buf[64], page_buf[64];

    display_header("Memory details");

    format_bytes(info->total_memory, total_buf, sizeof(total_buf));
    format_bytes(info->used_memory, used_buf, sizeof(used_buf));
    format_bytes(info->available_memory, free_buf, sizeof(free_buf));
    format_bytes(info->total_virtual, virtual_buf, sizeof(virtual_buf));
    format_bytes(info->page_file_total, page_buf, sizeof(page_buf));

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Physical memory: \n");
    reset_text_color();
    printf("  Total: %s\n", total_buf);
    printf("  Used: %s (%.1f%%)\n", used_buf, info->memory_usage_percent);
    printf("  Available: %s (%.1f%%)\n\n", free_buf, 100.0 - info->memory_usage_percent);

    printf("Memory usage: \n");
    draw_progress_bar(0, 9, 60, info->memory_usage_percent, COLOR_PROGRESS_FILL);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("\n\nVirtual memory: \n");
    reset_text_color();
    printf("  Total: %s\n", virtual_buf);
    format_bytes(info->available_virtual, free_buf, sizeof(free_buf));
    printf("  Available: %s\n\n", free_buf);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("System information: \n");
    reset_text_color();
    printf("  Page size: %lu bytes\n", info->sys_info.dwPageSize);
    printf("  Processor architechture: %u\n", info->sys_info.wProcessorArchitecture);
    printf("  Number of processors: %lu\n", info->sys_info.dwNumberOfProcessors);
}

void display_detailed_process_info(ProcessInfo* info, UIState* ui_state) {
    if (ui_state->selected_process_index >= info->count) return;

    Process* proc = &info->processes[ui_state->selected_process_index];
    char buffer[256];

    display_header("Detailed process information");

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Process name: ");
    reset_text_color();
    printf("%s\n", proc->name);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Process ID: ");
    reset_text_color();
    printf("%lu\n", proc->pid);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Parent PID: ");
    reset_text_color();
    printf("%lu\n", proc->parent_pid);

    if (strlen(proc->path) > 0) {
        set_text_color(UI_COLOR_HIGHLIGHT);
        printf("Path: ");
        reset_text_color();
        printf("%s\n", proc->path);
    }

    format_bytes(proc->memory_usage, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Working set: ");
    reset_text_color();
    printf("%s\n", buffer);

    format_bytes(proc->virtual_memory, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Private bytes: ");
    reset_text_color();
    printf("%s\n", buffer);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Thread count: ");
    reset_text_color();
    printf("%lu\n", proc->thread_count);
    
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Handle count: ");
    reset_text_color();
    printf("%lu\n", proc->handle_count);

    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Priority class: ");
    reset_text_color();
    printf("%s\n", get_priority_class_string(proc->priority_class));

    format_time(&proc->creation_time, buffer, sizeof(buffer));
    set_text_color(UI_COLOR_HIGHLIGHT);
    printf("Creation time: ");
    reset_text_color();
    printf("%s\n", buffer);
}

void display_menu(UIState* ui_state) {
    const char* views[] = {"System overview", "Process list", "Memory details", "Process details"};

    printf("\n");
    draw_horizontal_line(0, 0, 70);
    set_text_color(COLOR_HEADER);
    printf("MENU - Current view: %s\n", views[ui_state->current_view]);
    reset_text_color();

    printf("%s1. System overview    %s2. Process list    %s3. Memory details\n",
        ui_state->current_view == VIEW_OVERVIEW ? "> " : "  ",
        ui_state->current_view == VIEW_PROCESSES ? "> " : "  ",
        ui_state->current_view == VIEW_MEMORY ? "> " : "  ");

    printf("%s4. Process details    %sR. Refresh        %sQ. Quit\n",
        ui_state->current_view == VIEW_DETAILED_PROCESSES ? "> " : "  ",
        ui_state->auto_refresh ? "[ON] " : "[OFF] ",
        "");

    if (ui_state->current_view == VIEW_PROCESSES) {
        printf("\nProcess controls: S=Sort, F=Filter, UP/DOWN=Navigate, ENTER=Details\n");
    }
}

void display_header(const char* title) {
    set_text_color(COLOR_HEADER);
    printf("--- %s ---\n\n", title);
    reset_text_color();
}

void display_status_bar(UIState* ui_state) {
    set_cursor_position(0, g_console_state.screen_height - 1);
    set_text_color(COLOR_HEADER);
    printf("Auto-Refresh: %s | Interval: %dms | Press H for help",
        ui_state->auto_refresh ? "ON" : "OFF",
        ui_state->refersh_interval);
    reset_text_color();
}

void display_help() {
    clear_screen();
    display_header("HELP - System Information Monitor for Windows");

    printf("Navigation\n");
    printf("  1-4    - Switch between views\n");
    printf("  Q      - Quit application\n");
    printf("  R      - Toggle auto-refresh\n");
    printf("  H      - Show this help\n\n");

    printf("Process List View:\n");
    printf("  S          - Toggle sort criteria (Memory/CPU/Name/PID)\n");
    printf("  F          - Toggle system process filter\n");
    printf("  UP/DOWN    - Navigate process list\n");
    printf("  ENTER      - View detailed process information\n\n");

    printf("Memory view\n");
    printf("  Shows detailed memory usage statistics\n");
    printf("  Includes physical, virtual, and page file information\n\n");

    printf("Press any key to return...");
    _getch();
}

void draw_progress_bar(int x, int y, int width, double percent, WORD color) {
    int filled = (int)((percent / 100.0) * width);

    set_cursor_position(x, y);
    printf("[");

    set_text_color(color);
    for (int i = 0; i < filled; i++) {
        printf("#");
    }

    set_text_color(COLOR_PROGRESS_EMPTY);
    for (int i = filled; i < width; i++) {
        printf(" ");
    }

    reset_text_color();
    printf("] %.1f%%", percent);
}

void draw_box(int x, int y, int width, int height) {
    set_cursor_position(x, y);
    printf("+");
    for (int i = 1; i < width - 1; i++) printf("-");
    printf("+");

    for (int i = 1; i < height - 1; i++) {
        set_cursor_position(x, y + i);
        printf("|");
        set_cursor_position(x + width - 1, y + i);
        printf("|");
    }

    set_cursor_position(x, y + height - 1);
    printf("+");
    for (int i = 1; i < width - 1; i++) printf("-");
    printf("+");
}

void draw_horizontal_line(int x, int y, int width) {
    set_cursor_position(x, y);
    for (int i = 0; i < width; i++) {
        printf("-");
    }
    printf("\n");
}

void draw_vertical_line(int x, int y, int height) {
    for (int i = 0; i < height; i++) {
        set_cursor_position(x, y + i);
        printf("|");
    }
}

void format_bytes(DWORDLONG bytes, char* buffer, int buffer_size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = (double)bytes;

    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }

    if (unit == 0) {
        snprintf(buffer, buffer_size, "%.0f %s", size, units[unit]);
    } else {
        snprintf(buffer, buffer_size, "%.2f %s", size, units[unit]);
    }
}

void format_uptime(DWORD seconds, char* buffer, int buffer_size) {
    int days = seconds / 86400;
    int hours = (seconds % 86400) / 3600;
    int mins = (seconds % 3600) / 60;
    int secs = seconds % 60;

    if (days > 0) {
        snprintf(buffer, buffer_size, "%dd %02dh %02dm %02ds", days, hours, mins, secs);
    } else if (hours > 0) {
        snprintf(buffer, buffer_size, "%02dh %02dm %02ds", hours, mins, secs);
    } else {
        snprintf(buffer, buffer_size, "%02dm %02ds", mins, secs);
    }
}

void format_time(FILETIME* ft, char* buffer, int buffer_size) {
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);
    snprintf(buffer, buffer_size, "%02d/%02d/%04d %02d:%02d:%02d",
                st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
}

void format_percentage(double percent, char* buffer, int buffer_size) {
    snprintf(buffer, buffer_size, "%.1f%%", percent);
}

void format_number(DWORD number, char* buffer, int buffer_size) {
    snprintf(buffer, buffer_size, "%lu", number);
}

int handle_input(UIState* ui_state) {
    if (!_kbhit()) return 1;

    char key = _getch();

    // Handle special keys (arrows, function keys)
    if (key == 224) {
        char arrow = _getch();
        switch (arrow) {
            case 72: // Up arrow
                if (ui_state->current_view == VIEW_PROCESSES && ui_state->selected_process_index > 0) {
                    ui_state->selected_process_index--;
                }
                break;
            case 80: // Down arrow
                if (ui_state->current_view == VIEW_PROCESSES) {
                    ui_state->selected_process_index++;
                }
                break;
            // Add other arrow keys if needed
        }
        return 1;
    }

    switch (key) {
        case '1':
            ui_state->current_view = VIEW_OVERVIEW;
            break;
        case '2':
            ui_state->current_view = VIEW_PROCESSES;
            break;
        case '3':
            ui_state->current_view = VIEW_MEMORY;
            break;
        case '4':
            ui_state->current_view = VIEW_DETAILED_PROCESSES;
            break;
        case 'r':
        case 'R':
            ui_state->auto_refresh = !ui_state->auto_refresh;
            break;
        case 'h':
        case 'H':
            display_help();
            break;
        case 's':
        case 'S':
            if (ui_state->current_view == VIEW_PROCESSES) {
                toggle_sort_criteria(ui_state);
            }
            break;
        case 13:
            if (ui_state->current_view == VIEW_PROCESSES) {
                ui_state->current_view = VIEW_DETAILED_PROCESSES;
            }
            break;
        case 'q':
        case 'Q':
        case 27:
            return 0;
    }

    return 1;
}

void handle_process_navigation(UIState* ui_state, char key) {
    switch (key) {
        case 'h':
            if (ui_state->selected_process_index > 0) {
                ui_state->selected_process_index--;
            }
            break;
        case 80:
            ui_state->selected_process_index++;
            break;
    }
}

void toggle_sort_criteria(UIState* ui_state) {
    ui_state->process_sort_criteria = (ui_state->process_sort_criteria + 1) % 4;
}

void toggle_system_processes(UIState* ui_state) {
    ui_state->show_system_processes = !ui_state->show_system_processes;
}