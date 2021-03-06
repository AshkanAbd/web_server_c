#include "outgoing_response.h"

void change_max(int max) {
    MAX = max;
}

/**
 * Internal function that initialize OutgoingResponse from given data.
 * @param response
 * @param data
 * @param data_size
 */
void init(OutgoingResponse *response, void *data, int data_size) {
    response->data_size = data_size;

    response->data = malloc(response->data_size);
    memset(response->data, 0x1C, response->data_size);

    memcpy(response->data, data, response->data_size);
}

void init_ok(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_OK;

    init(response, data, data_size);
}

void init_not_found(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_NOT_FOUND;

    init(response, data, data_size);
}

void init_bad_request(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_BAD_REQUEST;

    init(response, data, data_size);
}

void init_invalid_action(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_INVALID_ACTION;

    init(response, data, data_size);
}

void init_server_error(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_SERVER_ERROR;

    init(response, data, data_size);
}

void init_invalid_syntax(OutgoingResponse *response, void *data, int data_size) {
    response->status = RESPONSE_INVALID_SYNTAX;

    init(response, data, data_size);
}

void send_to_client(OutgoingResponse *response, Client *client, int final_packet) {
    char *buffer = malloc(response->data_size + 3);
    memset(buffer, 0x1D, response->data_size + 3);

    *buffer = (char) response->status;
    if (final_packet) {
        *(buffer + 1) = 0x1C;
    }
    memcpy(buffer + 2, response->data, response->data_size);
    *(buffer + response->data_size + 2) = 0x1C;

    send(*client->socket, buffer, response->data_size + 3, 0);
    free(buffer);
}
