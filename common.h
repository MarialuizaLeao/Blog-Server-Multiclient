#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE 4
#define EXIT 5
#define UNSUBSCRIBE 6

#define BUFFER_SIZE 1024
#define MAX_LEN_TOPIC 50
#define MAX_LEN_CONTENT 2048
#define MAX_USERS 10

struct BlogOperation {
    int client_id;
    int operation_type;
    int server_response;
    char topic[50];
    char content[2048];
};

struct BlogOperation initBlogOperation(int client_id, int operation_type, char *topic, char *content){
    struct BlogOperation blogOperation;
    blogOperation.client_id = client_id;
    blogOperation.operation_type = operation_type;
    blogOperation.server_response = 0;
    strcpy(blogOperation.topic, topic);
    strcpy(blogOperation.content, content);
    return blogOperation;
}
