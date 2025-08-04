#include "http_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int http_init(void) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Error initializing the network stack from Winsock.\n");
        return -1;
    }
    return 0;
}

void http_cleanup(void) {
    WSACleanup();
}

int parse_url(const char* url, url_components_t* components) {
    if (!url || !components) return -1;

    memset(components, 0, sizeof(url_components_t));

    const char* scheme_end = strstr(url, "://");
    if (!scheme_end) {
        strcpy(components->scheme, "http");
        scheme_end = url - 3;
    } else {
        size_t scheme_len = scheme_end - url;
        if (scheme_len >= sizeof(components->scheme)) return -1;
        strncpy(components->scheme, url, scheme_len);
        components->scheme[scheme_len] = '\0';
    }

    const char* host_start = scheme_end + 3;
    const char* path_start = strchr(host_start, '/');
    const char* port_start = strchr(host_start, ':');

    if (port_start && (!path_start || port_start < path_start)) {
        size_t host_len = port_start - host_start;
        if (host_len >= sizeof(components->host)) return -1;
        strncpy(components->host, host_start, host_len);
        components->host[host_len] = '\0';

        components->port = atoi(port_start + 1);
    } else {
        const char* host_end = path_start ? path_start : (host_start + strlen(host_start));
        size_t host_len = host_end - host_start;
        if (host_len >= sizeof(components->host)) return -1;
        strncpy(components->host, host_start, host_len);
        components->host[host_len] = '\0';

        components->port = (strcmp(components->scheme, "https") == 0) ? DEFAULT_HTTPS_PORT : DEFAULT_HTTP_PORT;
    }

    if (path_start) {
        strncpy(components->path, path_start, sizeof(components->path) - 1);
    } else {
        strcpy(components->path, "/");
    }

    return 0;
}

int http_connect(http_connection_t* conn, const char* host, int port, int timeout_ms) {
    if (!conn || !host) return -1;

    memset(conn, 0, sizeof(http_connection_t));
    conn->timeout_ms = timeout_ms;

    conn->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn->socket == INVALID_SOCKET) {
        return -1;
    }

    DWORD timeout = timeout_ms;
    setsockopt(conn->socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(conn->socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    struct hostent* host_entry = gethostbyname(host);
    if (!host_entry) {
        closesocket(conn->socket);
        return -1;
    }

    conn->server_addr.sin_family = AF_INET;
    conn->server_addr.sin_port = htons(port);
    memcpy(&conn->server_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);

    if (connect(conn->socket, (struct sockaddr*)&conn->server_addr, sizeof(conn->server_addr)) == SOCKET_ERROR) {
        closesocket(conn->socket);
        return -1;
    }

    conn->is_connected = 1;
    return 0;
}

void http_disconnect(http_connection_t* conn) {
    if (conn && conn->is_connected) {
        shutdown(conn->socket, SD_BOTH);
        closesocket(conn->socket);
        conn->is_connected = 0;
    }
}

int http_send_request(http_connection_t* conn, const char* method, const char* path, const char* host, const char* headers) {
    if (!conn || !conn->is_connected || !method || !path || !host) return -1;

    char request[4096];
    int len = snprintf(request, sizeof(request),
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: WebScraper/1.0\r\n"
        "Connection: close\r\n"
        "%s"
        "\r\n",
        method, path, host, headers ? headers : "");

    if (len >= sizeof(request)) return -1;

    int total_sent = 0;
    while (total_sent < len) {
        int sent = send(conn->socket, request + total_sent, len - total_sent, 0);
        if (sent == SOCKET_ERROR) return -1;
        total_sent += sent;
    }

    return total_sent;
}

int http_receive_response(http_connection_t* conn, http_response_t* response) {
    if (!conn || !conn->is_connected || !response) return -1;

    response->capacity = 4096;
    response->data = malloc(response->capacity);
    response->size = 0;

    if (!response->data) return -1;

    char buffer[1024];
    int received;

    while ((received = recv(conn->socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        if (response->size + received >= response->capacity) {
            response->capacity *= 2;
            char* new_data = realloc(response->data, response->capacity);
            if (!new_data) {
                free(response->data);
                return -1;
            }
            response->data = new_data;
        }

        memcpy(response->data + response->size, buffer, received);
        response->size += received;
    }

    if (response->size < response->capacity) {
        response->data[response->size] = '\0';
    }

    return (received == SOCKET_ERROR && WSAGetLastError() != WSAECONNRESET) ? -1 : 0;
}

void http_response_free(http_response_t* response) {
    if (response && response->data) {
        free(response->data);
        response->data = NULL;
        response->size = 0;
        response->capacity = 0;
    }
}