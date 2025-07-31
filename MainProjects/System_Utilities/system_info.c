#include "system_info.h"

static CPUTimes g_previous_cpu_times = {0};
static int g_cpu_initialized = 0;

void update_system_info(SystemInfo* info) {
    if (!info) return;

    memset(info, 0, sizeof(SystemInfo));

    get_system_stats(info);
    get_memory_info(info);
    get_computer_name_info(info);
    get_uptime_info(info);
    info->cpu_usage = get_cpu_usage();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        THREADENTRY32 te32;

        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &pe32)) {
            do {
                info->process_count++;
            } while (Process32Next(snapshot, &pe32));
        }

        te32.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(snapshot, &te32)) {
            do {
                info->thread_count++;
            } while (Thread32Next(snapshot, &te32));
        }

        CloseHandle(snapshot);
    }
}

double get_cpu_usage() {
    CPUTimes current_times;
    FILETIME idle_time, kernel_time, user_time;

    if (!GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        return 0.0;
    }

    current_times.idle_time.LowPart = idle_time.dwLowDateTime;
    current_times.idle_time.HighPart = idle_time.dwHighDateTime;
    current_times.kernel_time.LowPart = kernel_time.dwLowDateTime;
    current_times.kernel_time.HighPart = kernel_time.dwHighDateTime;
    current_times.user_time.LowPart = user_time.dwLowDateTime;
    current_times.user_time.HighPart = user_time.dwHighDateTime;

    if (!g_cpu_initialized) {
        g_previous_cpu_times = current_times;
        g_cpu_initialized = 1;
        return 0.0;
    }

    ULONGLONG idle_diff = current_times.idle_time.QuadPart - g_previous_cpu_times.idle_time.QuadPart;
    ULONGLONG kernel_diff = current_times.kernel_time.QuadPart - g_previous_cpu_times.kernel_time.QuadPart;
    ULONGLONG user_diff = current_times.user_time.QuadPart - g_previous_cpu_times.user_time.QuadPart;

    ULONGLONG system_time = kernel_diff + user_diff;
    ULONGLONG busy_time = system_time - idle_diff;

    double cpu_usage = 0.0;

    if (system_time > 0) {
        cpu_usage = (double)busy_time * 100.0 / (double)system_time;
    }

    g_previous_cpu_times = current_times;

    return cpu_usage;
}

void get_memory_info(SystemInfo* info) {
    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&mem_status)) {
        info->total_memory = mem_status.ullTotalPhys;
        info->available_memory = mem_status.ullAvailPhys;
        info->used_memory = info->total_memory - info->available_memory;
        info->memory_usage_percent = (double)info->used_memory * 100.0 / (double)info->total_memory;

        info->total_virtual = mem_status.ullTotalVirtual;
        info->available_virtual = mem_status.ullAvailVirtual;
        info->page_file_total = mem_status.ullTotalPageFile;
        info->page_file_available = mem_status.ullAvailPageFile;

    }
}

void get_system_stats(SystemInfo* info) {
    GetSystemInfo(&info->sys_info);
}

void get_computer_name_info(SystemInfo* info) {
    DWORD size = MAX_NAME_LEN;
    GetComputerNameA(info->computer_name, &size);

    size = MAX_NAME_LEN;
    GetUserNameA(info->username, &size);
}

void get_uptime_info(SystemInfo* info) {
    info->uptime_seconds = GetTickCount() / 1000;
}