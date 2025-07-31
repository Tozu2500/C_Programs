#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "sysmon.h"

typedef struct {
    double cpu_usage;
    DWORDLONG total_memory;
    DWORDLONG available_memory;
    DWORDLONG used_memory;
    double memory_usage_percent;
    DWORD process_count;
    DWORD thread_count;
    DWORD uptime_seconds;
    char computer_name[MAX_NAME_LEN];
    char username[MAX_NAME_LEN];
    SYSTEM_INFO sys_info;
    DWORDLONG total_virtual;
    DWORDLONG available_virtual;
    DWORDLONG page_file_total;
    DWORDLONG page_file_available;
} SystemInfo;

typedef struct {
    ULARGE_INTEGER idle_time;
    ULARGE_INTEGER kernel_time;
    ULARGE_INTEGER user_time;
} CPUTimes;

void update_system_info(SystemInfo* info);
double get_cpu_usage();
void get_memory_info(SystemInfo* info);
void get_system_stats(SystemInfo* info);
void get_computer_name_info(SystemInfo* info);
void get_uptime_info(SystemInfo* info);

#endif // SYSTEM_INFO_H