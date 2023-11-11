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
#include <semaphore.h>

#define ERROR -1
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

struct BlogOperation initBlogOperation(int client_id, int operation_type, char *topic, char *content, int server_response);
int addrparse(const char * addrstr, const char * portstr, struct sockaddr_storage * storage);
int server_sockaddr_init(const char * protocol_version, const char * portstr, struct sockaddr_storage * storage);
void logexit(const char * msg);
size_t receive_all(int socket, void * buffer, size_t length);
