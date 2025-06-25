#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define DEFAULT_PORT 8080
#define BACKLOG 10
#define MAX_THREADS 50

typedef struct {
    int client_id;
    struct sockaddr_in client_addr;
} client_info_t;

int create_server_socket(int port);
void run_server(int server_fd);
void* handle_client(void* arg);
void cleanup_handle_info(client_info_t* client_info);

extern pthread_mutex_t thread_count_mutex;
extern int active_threads;

#endif // SERVER_H