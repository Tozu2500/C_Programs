#include "process_info.h"

void update_process_info(ProcessInfo* info) {
    if (!info) return;

    memset(info, 0, sizeof(ProcessInfo));
    get_process_list();
    sort_processes_by_memory(info);
}

void get_process_list(ProcessInfo* info) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &pe32)) {
        CloseHandle(snapshot);
        return;
    }

    info->count = 0;
    info->total_processes = 0;
    info->total_threads = 0;
    info->total_memory_used = 0;

    do {
        info->total_processes++;

        if (info->count >= MAX_PROCESSES) continue;

        Process* proc = &info->processes[info->count];

        proc->pid = pe32.th32ProcessID;
        proc->parent_pid = pe32.th32ParentProcessID;
        strncpy(proc->name, pe32.szExeFile, MAX_NAME_LEN - 1);
        proc->name[MAX_NAME_LEN - 1] = '\0';
        proc->thread_count = pe32.cntThreads;

        proc->is_system_process = (proc->pid == 0 || proc->pid == 4 ||
                                strcmp(proc->name, "System") == 0 ||
                                strcmp(proc->name, "System Idle Process") == 0);

        if (get_process_memory_info(proc->pid, &proc->memory_usage, &proc->virtual_memory)) {
            info->total_memory_used += proc->memory_usage;
        }

        proc->handle_count = get_process_handle_count(proc->pid);

        HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                            FALSE, proc->pid);
        if (process_handle) {
            FILETIME creation_time, exit_time, kernel_time, user_time;
            if (GetProcessTimes(process_handle, &creation_time, &exit_time,
                                &kernel_time, &user_time)) {
                                    proc->creation_time = creation_time;
                                    proc->kernel_time.LowPart = kernel_time.dwLowDateTime;
                                    proc->kernel_time.HighPart = kernel_time.dwHighDateTime;
                                    proc->user_time.LowPart = user_time.dwLowDateTime;
                                    proc->user_time.HighPart = user_time.dwHighDateTime;
                                }

                                proc->priority_class = GetPriorityClass(process_handle);
            
            char module_path[MAX_PATH];
            if (GetModuleFileNameExA(process_handle, NULL, module_path, MAX_PATH)) {
                strncpy(proc->path, module_path, MAX_PATH_LEN - 1);
                proc->path[MAX_PATH_LEN - 1] = '\0';
            }

            CloseHandle(process_handle);
        }

        info->total_threads += proc->thread_count;
        info->count++;

    } while (Process32Next(snapshot, &pe32));

    CloseHandle(snapshot);
}

void sort_processes_by_memory(ProcessInfo* info) {
    for (int i = 0; i < info->count - 1; i++) {
        for (int j = 0; j < info->count - i - 1; j++) {
            if (info->processes[j].memory_usage < info->processes[j + 1].memory_usage) {
                Process temp = info->processes[j];
                info->processes[j] = info->processes[j + 1];
                info->processes[j + 1] = temp;
            }
        }
    }
}

void sort_process_by_cpu(ProcessInfo* info) {
    for (int i = 0; i < info->count - 1; i++) {
        for (int j = 0; j < info->count - i - 1; j++) {
            if (info->processes[j].cpu_percent < info->processes[j + 1].cpu_percent) {
                Process temp = info->processes[j];
                info->processes[j] = info->processes[j + 1];
                info->processes[j + 1] = temp;
            }
        }
    }
}

void sort_processes_by_name(ProcessInfo* info) {
    for (int i = 0; i < info->count - 1; i++) {
        for (int j = 0; j < info->count - i - 1; j++) {
            if (strcmp(info->processes[j].name, info->processes[j + 1].name) > 0) {
                Process temp = info->processes[j];
                info->processes[j] = info->processes[j + 1];
                info->processes[j + 1] = temp;
            }
        }
    }
}

void sort_processes(ProcessInfo* info, SortCriteria criteria) {
    switch (criteria) {
        case SORT_BY_MEMORY:
            sort_processes_by_memory(info);
            break;
        case SORT_BY_CPU:
            sort_process_by_cpu(info);
            break;
        case SORT_BY_NAME:
            sort_processes_by_name(info);
            break;
        case SORT_BY_PID:
            for (int i = 0; i < info->count - 1; i++) {
                for (int j = 0; j < info->count - i - 1; j++) {
                    if (info->processes[j].pid > info->processes[j + 1].pid) {
                        Process temp = info->processes[j];
                        info->processes[j] = info->processes[j + 1];
                        info->processes[j + 1] = temp;
                    }
                }
            }
            break;
        case SORT_BY_THREADS:
            for (int i = 0; i < info->count - 1; i++) {
                for (int j = 0; j < info->count - i - 1; j++) {
                    if (info->processes[j].thread_count < info->processes[j + 1].thread_count) {
                        Process temp = info->processes[j];
                        info->processes[j] = info->processes[j + 1];
                        info->processes[j + 1] = temp;
                    }
                }
            }
            break;
    }
}

int get_process_memory_info(DWORD pid, DWORDLONG* memory_usage, DWORDLONG* virtual_memory) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!process) return 0;

    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        *memory_usage = pmc.WorkingSetSize;
        *virtual_memory = pmc.PrivateUsage;
        CloseHandle(process);
        return 1;
    }

    CloseHandle(process);
    return 0;
}

int get_process_thread_count(DWORD pid) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    int thread_count = 0;

    if (Thread32First(snapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                thread_count++;
            }
        } while (Thread32Next(snapshot, &te32));
    }

    CloseHandle(snapshot);
    return thread_count;
}

int get_process_handle_count(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!process) return 0;

    DWORD handle_count = 0;
    GetProcessHandleCount(process, &handle_count);
    CloseHandle(process);

    return handle_count;
}

char* get_priority_class_string(DWORD priority_class) {
    switch (priority_class) {
        case IDLE_PRIORITY_CLASS: return "Idle";
        case BELOW_NORMAL_PRIORITY_CLASS: return "Below normal";
        case NORMAL_PRIORITY_CLASS: return "Normal";
        case ABOVE_NORMAL_PRIORITY_CLASS: return "Above normal";
        case HIGH_PRIORITY_CLASS: return "High";
        case REALTIME_PRIORITY_CLASS: return "Realtime";
        default: return "Unknown";
    }
}

void filter_system_processes(ProcessInfo* info) {
    int filtered_count = 0;

    for (int i = 0; i < info->count; i++) {
        if (!info->processes[i].is_system_process) {
            if (filtered_count != i) {
                info->processes[filtered_count] = info->processes[i];
            }
            filtered_count++;
        }
    }

    info->count = filtered_count;
}