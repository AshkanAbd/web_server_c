#include "api.h"

/**
 * Internal function that prepares a client and socket to send request.
 * @param ip
 * @param port
 * @return
 */
Client *request_client(char *ip, int port) {
    ClientSocket *client_socket = malloc(sizeof(ClientSocket));
    memset(client_socket, 0, sizeof(ClientSocket));

    Client *client = malloc(sizeof(Client));
    memset(client, 0, sizeof(Client));

    init_socket(client_socket, ip, port);

    init_client(client, MAX, client_socket->socket_fd);

    return client;
}

/**
 * Internal function that combine a request to byte array for sending using socket.
 * @param request
 * @param buffer
 */
void prepare_request(OutgoingRequest *request, char *buffer) {
    memcpy(buffer, (char *) &request->action, 1);

    memcpy(buffer + 2, request->route, strlen(request->route));

    if (request->param_size != 0 && request->body_size != 0) {

        memcpy(buffer + 2 + strlen(request->route) + 1, request->param, request->param_size);

        memcpy(buffer + 2 + strlen(request->route) + 1 + request->param_size + 1,
               request->body, request->body_size);

    } else if (request->param_size != 0 && request->body_size == 0) {

        memcpy(buffer + 2 + strlen(request->route) + 1, request->param, request->param_size);

    } else if (request->param_size == 0 && request->body_size != 0) {

        memcpy(buffer + 2 + strlen(request->route) + 1 + 1, request->body, request->body_size);

    }
}

/**
 * Internal function that tells web server to send another packet.
 * @param client
 */
void request_next_packet(Client *client) {
    char *temp = malloc(1);
    *temp = '0';
    send(client->socket, temp, 1, 0);
    free(temp);
}

struct IncomingResponse *send_request(OutgoingRequest *request, Client *client) {
    int request_buffer_size = 1 + 1 + (int) strlen(request->route) + 1 + request->param_size +
                              1 + request->body_size + 1;
    char *request_buffer = malloc(request_buffer_size + 1);
    memset(request_buffer, 0x1D, request_buffer_size);
    *(request_buffer + request_buffer_size) = 0;

    prepare_request(request, request_buffer);

    send(client->socket, request_buffer, request_buffer_size, 0);
    free(request_buffer);
    free(request);

    unsigned long long int response_buffer_size = 0;
    unsigned long long int response_buffer_index;
    int current_buffer_size;
    char *response_buffer = malloc(response_buffer_size);
    memset(response_buffer, 0, response_buffer_size);
    char *current_buffer = NULL;
    int count = 0;

    do {
        count++;

        if (current_buffer != NULL) {
            free(current_buffer);
        }

        current_buffer_size = client->buffer_size;
        current_buffer = malloc(current_buffer_size);
        memset(current_buffer, 0, current_buffer_size);

        recv(client->socket, current_buffer, current_buffer_size, 0);
        if (strlen(current_buffer) == 0) {
            return NULL;
        }

        int wrong_packet_count = 0;
        register int i;
        for (i = current_buffer_size - 1; i != 0; i--, wrong_packet_count++) {
            if (*(current_buffer + i) == 0x1C) {
                wrong_packet_count++;
                break;
            }
        }

        current_buffer_size -= wrong_packet_count;
        response_buffer_index = response_buffer_size;
        response_buffer_size += current_buffer_size;

        if (count == 1) {
            response_buffer = realloc(response_buffer, response_buffer_size);
            memset(response_buffer + response_buffer_index, 0, current_buffer_size);
            memcpy(response_buffer + response_buffer_index, current_buffer, current_buffer_size);
            if (*(current_buffer + 1) == 0x1C) {
                break;
            }
        } else {
            current_buffer_size -= 2;
            response_buffer_size -= 2;
            response_buffer = realloc(response_buffer, response_buffer_size);
            memset(response_buffer + response_buffer_index, 0, current_buffer_size);
            memcpy(response_buffer + response_buffer_index, current_buffer + 2, current_buffer_size);
            if (*(current_buffer + 1) == 0x1C) {
                break;
            }
        }
        request_next_packet(client);
    } while (1);
    free(client);
    free(current_buffer);

    struct IncomingResponse *response = malloc(sizeof(struct IncomingResponse));
    memset(response, 0, sizeof(struct IncomingResponse));
    if (*response_buffer == RESPONSE_OK) {
        init_ok(response, response_buffer, response_buffer_size);
    } else if (*response_buffer == RESPONSE_NOT_FOUND) {
        init_not_found(response, response_buffer, response_buffer_size);
    } else if (*response_buffer == RESPONSE_BAD_REQUEST) {
        init_bad_request(response, response_buffer, response_buffer_size);
    } else if (*response_buffer == RESPONSE_INVALID_ACTION) {
        init_invalid_action(response, response_buffer, response_buffer_size);
    } else if (*response_buffer == RESPONSE_SERVER_ERROR) {
        init_server_error(response, response_buffer, response_buffer_size);
    } else if (*response_buffer == RESPONSE_INVALID_SYNTAX) {
        init_invalid_syntax(response, response_buffer, response_buffer_size);
    }
    free(response_buffer);

    return response;
}

struct IncomingResponse *api_read(char *route, void *param, int param_size, char *ip, int port) {
    Client *client = request_client(ip, port);

    OutgoingRequest *request = malloc(sizeof(OutgoingRequest));
    memset(request, 0, sizeof(OutgoingRequest));

    init_read(request, route, param, param_size);
    return send_request(request, client);
}

struct IncomingResponse *
api_create(char *route, void *param, int param_size, void *body, int body_size, char *ip, int port) {
    Client *client = request_client(ip, port);

    OutgoingRequest *request = malloc(sizeof(OutgoingRequest));
    memset(request, 0, sizeof(OutgoingRequest));

    init_create(request, route, param, param_size, body, body_size);

    return send_request(request, client);
}

struct IncomingResponse *
api_update(char *route, void *param, int param_size, void *body, int body_size, char *ip, int port) {
    Client *client = request_client(ip, port);

    OutgoingRequest *request = malloc(sizeof(OutgoingRequest));
    memset(request, 0, sizeof(OutgoingRequest));

    init_update(request, route, param, param_size, body, body_size);

    return send_request(request, client);
}

struct IncomingResponse *api_delete(char *route, void *param, int param_size, char *ip, int port) {
    Client *client = request_client(ip, port);

    OutgoingRequest *request = malloc(sizeof(OutgoingRequest));
    memset(request, 0, sizeof(OutgoingRequest));

    init_delete(request, route, param, param_size);

    return send_request(request, client);
}
