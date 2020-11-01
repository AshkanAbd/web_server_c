#include "router.h"

void readAction(struct Pipeline *pipeline, char *route,
                struct OutgoingResponse *(*func)(void *)) {
    initRouteTemplate((pipeline->templates + pipeline->route_count++), REQUEST_READ, route, func);
}

void createAction(struct Pipeline *pipeline, char *route,
                  struct OutgoingResponse *(*func)(void *)) {
    initRouteTemplate((pipeline->templates + pipeline->route_count++), REQUEST_CREATE, route, func);
}

void updateAction(struct Pipeline *pipeline, char *route,
                  struct OutgoingResponse *(*func)(void *)) {
    initRouteTemplate((pipeline->templates + pipeline->route_count++), REQUEST_UPDATE, route, func);
}

void deleteAction(struct Pipeline *pipeline, char *route,
                  struct OutgoingResponse *(*func)(void *)) {
    initRouteTemplate((pipeline->templates + pipeline->route_count++), REQUEST_DELETE, route, func);
}