#ifndef HTTP_H
#define HTTP_H

#include <sys/types.h>

#define BUFFER_SIZE 4096
#define METHOD_SIZE 16
#define URI_SIZE 512
#define VERSION_SIZE 16
#define HEADER_SIZE 1024

typedef struct {
    char method[METHOD_SIZE];
    char uri[URI_SIZE];
    char version[VERSION_SIZE];
    char headers[HEADER_SIZE];
    char* body;
    size_t body_length;
} http_request_t;

typedef struct {
    int status_code;
    char* content_type;
    char* body;
    size_t body_length;
    char* headers;
} http_response_t;

int parse_http_request(const char* raw_request, http_request_t* request);
void process_http_request(int client_fd, const char* raw_request);
void send_http_response(int client_fd, http_response_t* response);