#include "sysmon.h"
#include "ui.h"
#include "system_info.h"
#include "process_info.h"

void initialize_ui_state(UIState* ui_state) {
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_view = VIEW_OVERVIEW;
    ui_state->process_sort_criteria = SORT_BY_MEMORY;
    ui_state->show_system_processes = 1;
    ui_state->auto_refresh = 1;
    ui_state->refersh_interval = UPDATE_INTERVAL;
    ui_state->selected_process_index = 0;
    ui_state->process_list_offset = 0;
    ui_state->show_detailed_info = 0;
}

void update_process_sorting(ProcessInfo* proc_info, UIState* ui_state) {
    switch (ui_state->process_sort_criteria) {
        case SORT_BY_MEMORY:
            sort_processes_by_memory(proc_info);
            break;
        case SORT_BY_CPU:
            sort_processes_by_cpu(proc_info);
            break;
        case SORT_BY_NAME:
            sort_processes_by_name(proc_info);
            break;
        case SORT_BY_PID:
            sort_processes(proc_info, SORT_BY_PID);
            break;
        case SORT_BY_THREADS:
            sort_processes(proc_info, SORT_BY_THREADS);
            break;
    }

    if (!ui_state->show_system_processes) {
        filter_system_processes(proc_info);
    }
}

void display_current_view(SystemInfo* sys_info, ProcessInfo* proc_info, UIState* ui_state) {
    switch (ui_state->current_view) {
        case VIEW_OVERVIEW:
            display_system_overview(sys_info);
            break;
        case VIEW_PROCESSES:
            update_process_sorting(proc_info, ui_state);
            display_process_list(proc_info, ui_state);
            break;
        case VIEW_MEMORY:
            display_memory_info(sys_info);
            break;
        case VIEW_DETAILED_PROCESSES:
            update_process_sorting(proc_info, ui_state);
            display_detailed_process_info(proc_info, ui_state);
            break;
    }
}

void validate_process_selection(ProcessInfo* proc_info, UIState* ui_state) {
    if (ui_state->selected_process_index >= proc_info->count) {
        ui_state->selected_process_index = proc_info->count - 1;
    }

    if (ui_state->selected_process_index < 0) {
        ui_state->selected_process_index = 0;
    }
}

void display_loading_message() {
    clear_screen();
    set_text_color(COLOR_HIGHLIGHT);
    printf("System Information Monitor - For windows\n");
    printf("Starting...\n");
    reset_text_color();
    Sleep(500);
}

void display_exit_message() {
    clear_screen();
    set_text_color(COLOR_SUCCESS);
    printf("Thank you for using the app!\n");
    printf("Exiting...\n");
    reset_text_color();
    Sleep(1000);
}

int main() {
    SystemInfo sys_info;
    ProcessInfo proc_info;
    UIState ui_state;
    int running = 1;
    DWORD last_update = 0;

    display_loading_message();

    init_console();
    initialize_ui_state(&ui_state);

    update_system_info(&sys_info);
    update_process_info(&proc_info);

    while (running) {
        DWORD current_time = GetTickCount();

        if (ui_state.auto_refresh && (current_time - last_update) >= ui_state.refersh_interval) {
            update_system_info(&sys_info);
            update_process_info(&proc_info);
            last_update = current_time;
        }

        clear_screen();

        validate_process_selection(&proc_info, &ui_state);
        display_current_view(&sys_info, &proc_info, &ui_state);
        display_menu(&ui_state);

        running = handle_input(&ui_state);

        if (!ui_state.auto_refresh) {
            update_system_info(&sys_info);
            update_process_info(&proc_info);
        }

        if (ui_state.auto_refresh) {
            Sleep(100);
        } else {
            Sleep(50);
        }
    }

    cleanup_console();
    display_exit_message();

    return 0;
}