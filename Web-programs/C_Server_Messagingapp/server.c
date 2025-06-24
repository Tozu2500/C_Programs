#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <napenforcementclient.h>/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048
#define USERNAME_LEN 32

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char username[USERNAME_LEN];
} client_t;

static _Atomic int cli_count = 0;
static int uid = 10;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overlap_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void print_client_addr(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d.",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid != uid) {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0) {
                    perror("Error: write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mmutex_unlock(&clients_mutex);
}

void send_user_list() {
    char user_list[BUFFER_SIZE];
    strcpy(user_list, "Users online:\n");

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            strcat(user_list, " ");
            strcat(user_list, clients[i]->username);
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    strcat(user_list, "\n");

    pthread_message_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            write(clients[i]->sockfd, user_list, strlen(user_list));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    char buff_out[BUFFER_SIZE];
    char username[USERNAME_LEN];
    int leave_flag = 0;

    cli_count++;
    client_t *cli = (client_t *)arg;

    // Receive username
    if (recv(cli->sockfd, username, USERNAME_LEN, 0) <= 0 || strlen(username) < 2 || strlen(username) >= USERNAME_LEN - 1) {
        printf("Didn't enter the username.\n");
        leave_flag = 1;
    } else {
        strcpy(cli->username, username);
        sprintf(buff_out, "%s has joined the chat\n", cli->username);
        printf("%s", buff_out);
        send_message(buff_out, cli->uid);
        send_user_list();
    }

    bzero(buff_out, BUFFER_SIZE);

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->sockfd, buff_out, BUFFER_SIZE, 0);
        if (receive > 0) {
            if (strlen(buff_out) > 0) {
                // Check for special commands
                if (strncmp(buff_out, "/quit", 5) == 0) {
                    leave_flag = 1;
                } else if (strncmp(buff_out, "/users", 6) == 0) {
                    send_user_list();
                } else {
                    char message[BUFFER_SIZE + USERNAME_LEN + 10];
                    sprintf(message, "[%s]: %s", cli->username, buff_out);
                    send_message(message, cli->uid);
                    str_trim_lf(message, strlen(message));
                    printf("%s\n", message);
                }
            }
        } else if (receive == 0 || strcmp(buff_out, "/quit") == 0) {
            sprintf(buff_out, "%s has left the chat\n", cli->username);
            printf("%s", buff_out);
            send_message(buff_out, cli->uid);
            leave_flag = 1;
        } else {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buff_out, BUFFER_SIZE);
    }

    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    send_user_list();
    pthread_detach(pthread_self());

    return NULL;
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    pthread_t tid;

    // Socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("ERROR: Socket creation failed!\n");
        return EXIT_FAILURE;
    }

    // Socket options
    int option = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        perror("ERROR: setsockopt failed!\n");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Binding
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed!\n");
        return EXIT_FAILURE;
    }

    // Listening
    if (listen(sockfd, 10) < 0) {
        perror("ERROR: Listeneing failed!\n");
        return EXIT_FAILURE;
    }

    printf("=== Welcome to the Chat Server ===\n");
    printf("The server is listening on port %d\n", PORT);

    while (1) {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        // Check if the maximum number of clients is going to be reached after accepting a new client
        if ((cli_count + 1) == MAX_CLIENTS) {
            printf("Max clients reached. Connection rejected: ");
            print_client_addr(cli_addr);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }

        // Client settings
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = connfd;
        cli->uid = uid++;

        // Add client to the queue
        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void*)cli);

        // Reduce CPU usage
        sleep(1);
        
    }
    close(sockfd);

    return EXIT_SUCCESS;

}

