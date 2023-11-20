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
#include <pthread.h>

#define NEW_CONNECTION 1
#define NEW_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE 4
#define EXIT 5
#define UNSUBSCRIBE 6

#define SUCCESS 0
#define ERROR -1

#define ERROR_ALREADY_SUBSCRIBED -2
#define ERROR_NOT_SUBSCRIBED -3
#define ERROR_TOPIC_NOT_FOUND -4


#define BUFFER_SIZE 2048
#define MAX_USERS 10
#define MAX_TOPICS 10
#define MAX_POSTS 100

struct BlogOperation {
    int client_id;
    int operation_type;
    int server_response;
    char topic[50];
    char content[2048];
};

struct BlogOperation initBlogOperation(int client_id, int operation_type, char *topic, char *content, int server_response);
void logexit(const char * msg);
