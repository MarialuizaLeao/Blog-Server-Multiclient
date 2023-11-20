#include "common.h"

char *ip;
char *port;
struct Blog mediumBlog;

/* 

    Struct for representing a client and its thread

*/

struct Client
{
    int id;
    int sock;
    pthread_t thread;
};

/*

    Struct for representing a post

*/

struct Post
{
    int id;
    char content[BUFFER_SIZE];
    int madeBy;
};

/*

    Struct for representing a topic

*/

struct Topic
{
    int id;
    char name[50];
    bool subscribed[MAX_USERS];
    int subscribedCount;
    struct Post posts[MAX_POSTS];
    int postsCount;
};

/*

    Struct for representing the blog

*/

struct Blog
{
    struct Topic topics[MAX_TOPICS];
    struct Client clients[MAX_USERS];
    bool users[MAX_USERS];
    int topicsCount;
    int usersCount;
};

/*
    
        Functions for handling blog operations
    
*/

struct Client initClient(int id, int sock);
struct Post initPost(int id, char *content, int madeBy);
struct Topic initTopic(int id, char *name);
void initBlog();
int topicAlreadyExists(char *topicName);
void sendNotificationToSubscribers(struct Topic topic, int postMadeBy, char *content);
void addPost(int userId, char *content, char *topicName);
int subscribeToTopic(int userId, char *topicName);
int unsubscribeToTopic(int userId, char* topic);
int addNewUser(int sock);
void removeUser(int userId);
void getTopics(char *topics);
void initArgs(int argc, char *argv[]);
struct BlogOperation handleClientRequest(struct BlogOperation request);
void* clientFunction(void* clientThread);
int initServerSockaddr(const char *ipProtocol, const char *portstr, struct sockaddr_storage *storage);
int initSocket();
