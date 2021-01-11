#include "post.h"

void init_post_full(Post *post, int id, char *title, char *description, int user_id,
                    char *created_at, char *updated_at) {
    post->id = id;

    post->user_id = user_id;

    set_post_column(&post->title, title, 0);

    set_post_column(&post->description, description, 0);

    set_post_column(&post->created_at, created_at, 0);

    set_post_column(&post->updated_at, updated_at, 0);
}

void init_post(Post *post, char *title, char *description, int user_id) {
    post->user_id = user_id;

    set_post_column(&post->title, title, 0);

    set_post_column(&post->description, description, 0);
}

void set_post_column(char **column, const char *value, int with_free) {
    if (column == NULL) {
        return;
    }

    if (*column != NULL && with_free) {
        free(*column);
    }

    if (value != NULL) {
        *column = malloc(strlen(value) + 1);
        memset(*column, 0, strlen(value) + 1);
        memcpy(*column, value, strlen(value));
    } else {
        *column = NULL;
    }
}

int post_search_by_id(const char *id, void *ptr, int (*callback)(void *, int, char **, char **), char **msg) {
    char *base_search_sql = "SELECT * FROM posts WHERE id = ";
    size_t custom_search_sql_size = strlen(base_search_sql) + 1 + strlen(id) + 2;
    char *custom_search_sql = malloc(custom_search_sql_size);
    memset(custom_search_sql, 0, custom_search_sql_size);

    memcpy(custom_search_sql, base_search_sql, strlen(base_search_sql));

    *(custom_search_sql + strlen(base_search_sql)) = '\'';
    memcpy(custom_search_sql + strlen(base_search_sql) + 1, id, strlen(id));
    *(custom_search_sql + strlen(base_search_sql) + 1 + strlen(id)) = '\'';

    return sqlite3_exec(db_connection, custom_search_sql, callback, ptr, msg);
}