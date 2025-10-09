#ifndef THREADING_H
#define THREADING_H

#include "file_operations.h"
#include <windows.h>

#define MAX_QUEUE_SIZE 1024

typedef struct {
    char source[MAX_PATH_LENGTH];
    char destination[MAX_PATH_LENGTH];
    FileInfo info;
} CopyTask;

typedef struct {
    CopyTask tasks[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    CRITICAL_SECTION lock;
    HANDLE not_empty;
    HANDLE not_full;
    bool shutdown;
} TaskQueue;

typedef struct {
    HANDLE thread;
    DWORD thread_id;
    TaskQueue* queue;
    const CopyOptions* options;
    CopyStatistics* stats;
    bool active;
} WorkerThread;

typedef struct {
    WorkerThread workers[MAX_THREADS];
    int worker_count;
    TaskQueue queue;
    CopyStatistics combined_stats;
    CRITICAL_SECTION stats_lock;
} ThreadPool;

int init_task_queue(TaskQueue* queue);
int cleanup_task_queue(TaskQueue* queue);
int enqueue_task(TaskQueue* queue, const CopyTask* task);
int dequeue_task(TaskQueue* queue, CopyTask* task);
int init_thread_pool(ThreadPool* pool, int thread_count, const CopyOptions* options);
int cleanup_thread_pool(ThreadPool* pool);
DWORD WINAPI worker_thread_function(LPVOID param);
int submit_copy_task(ThreadPool* pool, const char* source, const char* dest, const FileInfo* info);

#endif