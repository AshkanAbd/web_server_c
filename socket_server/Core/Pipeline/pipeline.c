#include "pipeline.h"
#include "../Router/router.h"

void init_pipeline(struct Pipeline *pipeline) {
    pipeline->route_count = 0;

    pipeline->static_files_prefix = NULL;

    pipeline->templates = malloc(1024 * sizeof(struct RouteTemplate));
    memset(pipeline->templates, 0, 1024 * sizeof(struct RouteTemplate));

    registerRoutes(pipeline);
}

void set_root_dir(struct Pipeline *pipeline, const char *root_dir) {
    pipeline->static_files_prefix = malloc(strlen(root_dir) + 1);
    memset(pipeline->static_files_prefix, 0, strlen(root_dir) + 1);
    memcpy(pipeline->static_files_prefix, root_dir, strlen(root_dir));
}

struct RouteTemplate *match_request(struct Pipeline *pipeline, struct IncomingRequest *request) {
    struct RouteTemplate *template = NULL;
    register int i;
    for (i = 0; i < pipeline->route_count; ++i) {
        struct RouteTemplate *tmp = pipeline->templates + i;
        if (check_route(tmp, request)) {
            if (check_action(tmp, request)) {
                return tmp;
            }
            template = tmp;
        }
    }
    return template;
}

int check_route(struct RouteTemplate *template, struct IncomingRequest *request) {
    if (strcmp(template->route, request->route) == 0) {
        return 1;
    }
    return 0;
}

int check_action(struct RouteTemplate *template, struct IncomingRequest *request) {
    if (template->action == request->action) {
        return 1;
    }
    return 0;
}

struct OutgoingResponse *execute_controller(struct IncomingRequest *request, struct RouteTemplate *template) {
    return (*template->func)(request);
}

int request_static_files(struct Pipeline *pipeline, struct IncomingRequest *request) {
    if (request->action != RESPONSE_OK) {
        return 0;
    }
    char *file_path = malloc(strlen(pipeline->static_files_prefix) + strlen(request->route) + 1);
    memset(file_path, 0, strlen(pipeline->static_files_prefix) + strlen(request->route) + 1);
    memcpy(file_path, pipeline->static_files_prefix, strlen(pipeline->static_files_prefix));
    memcpy(file_path + strlen(pipeline->static_files_prefix), request->route, strlen(request->route));

    struct stat *stat_buffer = malloc(sizeof(struct stat));
    memset(stat_buffer, 0, sizeof(struct stat));

    stat(file_path, stat_buffer);
    if (S_ISREG(stat_buffer->st_mode)) {
        return 1;
    }
    return 0;
}

void serve_static_file(struct OutgoingResponse *response, struct Pipeline *pipeline, struct IncomingRequest *request,
                       struct Client *client) {
    char *file_path = malloc(strlen(pipeline->static_files_prefix) + strlen(request->route) + 1);
    memset(file_path, 0, strlen(pipeline->static_files_prefix) + strlen(request->route) + 1);
    memcpy(file_path, pipeline->static_files_prefix, strlen(pipeline->static_files_prefix));
    memcpy(file_path + strlen(pipeline->static_files_prefix), request->route, strlen(request->route));

    FILE *static_file = fopen(file_path, "rb");
    fseek(static_file, 0, SEEK_END);
    int file_size = ftell(static_file);
    fseek(static_file, 0, SEEK_SET);

    int max_each_response = MAX - 4;
    char *buffer;

    int current_read = 0;
    int count = 0;

    while (1) {
        response->status = RESPONSE_OK;
        response->data_size = max_each_response;
        response->data = malloc(max_each_response);
        memset(response->data, 0, max_each_response);

        buffer = malloc(max_each_response);
        memset(buffer, 0, max_each_response);

        fread(buffer, sizeof(char), max_each_response, static_file);

        memmove(response->data, buffer, max_each_response);
        count++;
        current_read += max_each_response;
        if (current_read >= file_size) {
            response->data_size = file_size - (current_read - max_each_response);
            send_to_client(response, client, 1);
            break;
        }
        send_to_client(response, client, 0);
        wait_for_client(client);
    }
    fclose(static_file);
    printf("sent packet count: %d\n", count);

    close_client(client);
}

void wait_for_client(struct Client *client){
    char *tmp = malloc(1);
    recv(*client->socket, tmp, 1, 0);
}
