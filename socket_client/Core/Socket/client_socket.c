#include "client_socket.h"

int create_socket(struct ClientSocket *s) {
    WSADATA Data;
    WSAStartup(MAKEWORD(2, 2), &Data);

    s->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s->socket_fd == -1) {
        return errno;
    }
    memset(&(s->server_address), 0, sizeof(s->server_address));
    return 0;
}

void assign_port(struct ClientSocket *s, char *ip, int port) {
    s->server_address.sin_family = AF_INET;
    s->server_address.sin_addr.s_addr = inet_addr(ip);
    s->server_address.sin_port = htons(port);
}

int connect_socket(struct ClientSocket *s) {
    if (connect(s->socket_fd, (struct sockaddr *) &s->server_address, sizeof(s->server_address)) != 0) {
        return errno;
    }
    return 0;
}

void close_socket(struct ClientSocket *s) {
    close(s->socket_fd);
    WSACleanup();
}