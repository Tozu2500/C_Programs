#include "threading.h"
#include <stdio.h>
#include <string.h>

int init_task_queue(TaskQueue* queue) {
    if (!queue) return -1;
    
    memset(queue, 0, sizeof(TaskQueue));
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    queue->shutdown = false;
    
    InitializeCriticalSection(&queue->lock);
    queue->not_empty = CreateEvent(NULL, FALSE, FALSE, NULL);
    queue->not_full = CreateEvent(NULL, FALSE, TRUE, NULL);
    
    if (!queue->not_empty || !queue->not_full) {
        return -1;
    }
    
    return 0;
}

int cleanup_task_queue(TaskQueue* queue) {
    if (!queue) return -1;
    
    EnterCriticalSection(&queue->lock);
    queue->shutdown = true;
    LeaveCriticalSection(&queue->lock);
    
    SetEvent(queue->not_empty);
    SetEvent(queue->not_full);
    
    if (queue->not_empty) CloseHandle(queue->not_empty);
    if (queue->not_full) CloseHandle(queue->not_full);
    
    DeleteCriticalSection(&queue->lock);
    
    return 0;
}

int enqueue_task(TaskQueue* queue, const CopyTask* task) {
    if (!queue || !task) return -1;
    
    EnterCriticalSection(&queue->lock);
    
    while (queue->count >= MAX_QUEUE_SIZE && !queue->shutdown) {
        LeaveCriticalSection(&queue->lock);
        WaitForSingleObject(queue->not_full, INFINITE);
        EnterCriticalSection(&queue->lock);
    }
    
    if (queue->shutdown) {
        LeaveCriticalSection(&queue->lock);
        return -1;
    }
    
    queue->tasks[queue->rear] = *task;
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->count++;
    
    LeaveCriticalSection(&queue->lock);
    SetEvent(queue->not_empty);
    
    return 0;
}

int dequeue_task(TaskQueue* queue, CopyTask* task) {
    if (!queue || !task) return -1;
    
    EnterCriticalSection(&queue->lock);
    
    while (queue->count == 0 && !queue->shutdown) {
        LeaveCriticalSection(&queue->lock);
        WaitForSingleObject(queue->not_empty, INFINITE);
        EnterCriticalSection(&queue->lock);
    }
    
    if (queue->shutdown && queue->count == 0) {
        LeaveCriticalSection(&queue->lock);
        return -1;
    }
    
    *task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->count--;
    
    LeaveCriticalSection(&queue->lock);
    SetEvent(queue->not_full);
    
    return 0;
}

DWORD WINAPI worker_thread_function(LPVOID param) {
    WorkerThread* worker = (WorkerThread*)param;
    if (!worker) return 1;
    
    worker->active = true;
    
    while (true) {
        CopyTask task;
        if (dequeue_task(worker->queue, &task) != 0) {
            break;
        }
        
        if (task.info.is_directory) {
            continue;
        }
        
        int result = copy_single_file(task.source, task.destination, worker->options);
        
        EnterCriticalSection(&((ThreadPool*)((char*)worker - 
            ((char*)&((ThreadPool*)0)->workers[0] - (char*)0)))->stats_lock);
        
        if (result == 0) {
            worker->stats->copied_files++;
            worker->stats->copied_bytes += task.info.size;
        } else {
            worker->stats->failed_files++;
        }
        
        LeaveCriticalSection(&((ThreadPool*)((char*)worker - 
            ((char*)&((ThreadPool*)0)->workers[0] - (char*)0)))->stats_lock);
    }
    
    worker->active = false;
    return 0;
}

int init_thread_pool(ThreadPool* pool, int thread_count, const CopyOptions* options) {
    if (!pool || !options || thread_count < 1 || thread_count > MAX_THREADS) return -1;
    
    memset(pool, 0, sizeof(ThreadPool));
    pool->worker_count = thread_count;
    
    if (init_task_queue(&pool->queue) != 0) {
        return -1;
    }
    
    InitializeCriticalSection(&pool->stats_lock);
    
    for (int i = 0; i < thread_count; i++) {
        pool->workers[i].queue = &pool->queue;
        pool->workers[i].options = options;
        pool->workers[i].stats = &pool->combined_stats;
        pool->workers[i].active = false;
        
        pool->workers[i].thread = CreateThread(NULL, 0, worker_thread_function,
                                               &pool->workers[i], 0,
                                               &pool->workers[i].thread_id);
        
        if (!pool->workers[i].thread) {
            cleanup_thread_pool(pool);
            return -1;
        }
    }
    
    return 0;
}

int cleanup_thread_pool(ThreadPool* pool) {
    if (!pool) return -1;
    
    cleanup_task_queue(&pool->queue);
    
    HANDLE threads[MAX_THREADS];
    int thread_count = 0;
    
    for (int i = 0; i < pool->worker_count; i++) {
        if (pool->workers[i].thread) {
            threads[thread_count++] = pool->workers[i].thread;
        }
    }
    
    if (thread_count > 0) {
        WaitForMultipleObjects(thread_count, threads, TRUE, INFINITE);
        
        for (int i = 0; i < thread_count; i++) {
            CloseHandle(threads[i]);
        }
    }
    
    DeleteCriticalSection(&pool->stats_lock);
    
    return 0;
}

int submit_copy_task(ThreadPool* pool, const char* source, const char* dest, const FileInfo* info) {
    if (!pool || !source || !dest || !info) return -1;
    
    CopyTask task;
    strncpy(task.source, source, MAX_PATH_LENGTH - 1);
    task.source[MAX_PATH_LENGTH - 1] = '\0';
    strncpy(task.destination, dest, MAX_PATH_LENGTH - 1);
    task.destination[MAX_PATH_LENGTH - 1] = '\0';
    task.info = *info;
    
    return enqueue_task(&pool->queue, &task);
}