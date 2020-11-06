#include "outgoing_response.h"

void init(struct OutgoingResponse *response, void *data, int data_size) {
    response->data_size = data_size;

    response->data = malloc(data_size);
    memset(response->data, 0x1C, data_size);
    memcpy(response->data, data, data_size);
}

void init_ok(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_OK;

    init(response, data, data_size);
}

void init_not_found(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_NOT_FOUND;

    init(response, data, data_size);
}

void init_bad_request(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_BAD_REQUEST;

    init(response, data, data_size);
}

void init_invalid_action(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_INVALID_ACTION;

    init(response, data, data_size);
}

void init_server_error(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_SERVER_ERROR;

    init(response, data, data_size);
}

void init_invalid_syntax(struct OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_INVALID_SYNTAX;

    init(response, data, data_size);
}

void send_to_client(struct OutgoingResponse *response, struct Client *client) {
    char *buffer = malloc(response->data_size + 4);
    memset(buffer, 0x1D, response->data_size + 4);

    *buffer = (char) response->status;
    *(buffer + 2) = 0x1C;
    memcpy(buffer + 4, response->data, response->data_size);

    send(*client->socket, buffer, response->data_size + 4, 0);
}

void send_body_to_client(struct OutgoingResponse *response, struct Client *client) {
    char *buffer = malloc(response->data_size);
    memset(buffer, 0x1D, response->data_size);

    memcpy(buffer, response->data, response->data_size);

    send(*client->socket, buffer, response->data_size, 0);
}
