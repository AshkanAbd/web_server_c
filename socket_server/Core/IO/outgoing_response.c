#include "outgoing_response.h"

void init(struct OutgoingResponse *response, void *data, int data_size) {
    response->data_size = data_size + 2;

    response->data = malloc(data_size + 2);
    memset(response->data, 0, data_size + 2);
    memcpy(response->data, data, data_size);
    *((char *) (response->data + data_size + 1)) = 0x1C;
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
    char *buffer = malloc(response->data_size + 2);
    memset(buffer, 0, response->data_size + 2);

    *buffer = (char) response->status;
    memcpy(buffer + 2, response->data, response->data_size);

    char *encoded_buffer = base64_encode(buffer, response->data_size + 2);

    printf("packet length %llu", strlen(encoded_buffer));

    send(*client->socket, encoded_buffer, strlen(encoded_buffer), 0);
}

void send_body_to_client(struct OutgoingResponse *response, struct Client *client) {
    char *buffer = malloc(response->data_size);
    memset(buffer, 0, response->data_size);

    memcpy(buffer, response->data, response->data_size);

    char *encoded_buffer = base64_encode(buffer, response->data_size + 2);

    printf("packet length %llu", strlen(encoded_buffer));

    send(*client->socket, encoded_buffer, strlen(encoded_buffer), 0);
}
