#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MAX_URL_LENGTH 2048
#define MAX_HOST_LENGTH 256
#define MAX_PATH_LENGTH 1024
#define MAX_RESPONSE_SIZE 65536
#define DEFAULT_HTTP_PORT 80
#define DEFAULT_HTTPS_PORT 443

typedef struct {
    char scheme[8];
    char host[MAX_HOST_LENGTH];
    int port;
    char path[MAX_PATH_LENGTH];
} url_components_t;

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} http_response_t;

typedef struct {
    SOCKET socket;
    struct sockaddr_in server_addr;
    int timeout_ms;
    int is_connected;
} http_connection_t;

int http_init(void);
void http_cleanup(void);
int parse_url(const char* url, url_components_t* components);
int http_connect(http_connection_t* conn, const char* host, int port, int timeout_ms);
void http_disconnect(http_connection_t* conn);
int http_send_request(http_connection_t* conn, const char* method, const char* path, const char* host, const char* headers);
int http_receive_response(http_connection_t* conn, http_response_t* response);
void http_response_free(http_response_t* response);
char* http_get_header_value(const char* response, const char* header_name);
char* http_get_body(const char* response);
int http_get_status_code(const char* response);

#endif // HTTP_CLIENT_H