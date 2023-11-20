#include "common.h"

struct BlogOperation initBlogOperation(int client_id, int operation_type, char *topic, char *content, int server_response){
    struct BlogOperation blogOperation;
    blogOperation.client_id = client_id;
    blogOperation.operation_type = operation_type;
    blogOperation.server_response = 0;
    strcpy(blogOperation.topic, topic);
    strcpy(blogOperation.content, content);
    return blogOperation;
}

void logexit(const char *str) {
  perror(str);
  exit(EXIT_FAILURE);
}