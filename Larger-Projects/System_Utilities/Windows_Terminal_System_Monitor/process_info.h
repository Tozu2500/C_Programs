#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include "sysmon.h"

typedef struct {
    DWORD pid;
    DWORD parent_pid;
    char name[MAX_NAME_LEN];
    char path[MAX_PATH_LEN];
    DWORDLONG memory_usage;
    DWORDLONG virtual_memory;
    DWORD thread_count;
    DWORD handle_count;
    double cpu_percent;
    DWORD priority_class;
    FILETIME creation_time;
    LARGE_INTEGER kernel_time;
    LARGE_INTEGER user_time;
    int is_system_process;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int count;
    int total_processes;
    DWORD total_threads;
    DWORDLONG total_memory_used;
} ProcessInfo;

typedef enum {
    SORT_BY_MEMORY,
    SORT_BY_CPU,
    SORT_BY_PID,
    SORT_BY_NAME,
    SORT_BY_THREADS
} SortCriteria;

void update_process_info(ProcessInfo* info);
void get_process_list(ProcessInfo* info);
void sort_processes_by_memory(ProcessInfo* info);
void sort_processes_by_cpu(ProcessInfo* info);
void sort_processes_by_name(ProcessInfo* info);
void sort_processes(ProcessInfo* info, SortCriteria criteria);
int get_process_memory_info(DWORD pid, DWORDLONG* memory_usage, DWORDLONG* virtual_memory);
int get_process_thread_count(DWORD pid);
int get_process_handle_count(DWORD pid);
char* get_priority_class_string(DWORD priority_class);
void filter_system_processes(ProcessInfo* info);

#endif // PROCESS_INFO_H