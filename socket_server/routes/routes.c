/**
 * The file that dynamic routes should be defined in it.
 */
#include "../Core/Router/router.h"
#include "../controllers/controllers.h"

/**
 * The function that dynamic routes should be defines in it.
 * Define your dynamic routes in this function.
 * @param pipeline
 */
void registerRoutes(Pipeline *pipeline) {
    createAction(pipeline, "/sign_in", sign_in);
    createAction(pipeline, "/sign_up", sign_up);

    readAction(pipeline, "/post_list", post_list);
    readAction(pipeline, "/get_post", get_post);
    createAction(pipeline, "/create_post", create_post);
    updateAction(pipeline, "/update_post", update_post);
    deleteAction(pipeline, "/delete_post", delete_post);
}
