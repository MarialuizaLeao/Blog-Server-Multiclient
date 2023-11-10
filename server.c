#include "common.h"

char *ip;
char *port;
struct blog mediumBlog;

struct post
{
    int id;
    char content[MAX_LEN_CONTENT];
    int madeBy;
};

struct post initPost(int id, char *content, int madeBy){
    struct post newPost;
    newPost.id = id;
    strcpy(newPost.content, content);
    newPost.madeBy = madeBy;
    return newPost;
}

struct topic
{
    int id;
    char name[50];
    bool subscribed[MAX_USERS];
    int subscribedCount;
    struct post posts[MAX_LEN_TOPIC];
    int postsCount;
};

struct topic initTopic(int id, char *name, struct post newPost)
{
    struct topic newTopic;

    newTopic.id = id;
    strcpy(newTopic.name, name);
    for(int i = 0; i < 10; i++){
        newTopic.subscribed[i] = false;
    }
    newTopic.subscribedCount = 0;
    newTopic.posts[0] = newPost; 
    newTopic.postsCount = 1;
    return newTopic;
}

struct blog
{
    struct topic topics[MAX_LEN_TOPIC];
    int topicsCount;
    bool users[MAX_USERS];
    int usersCount;
};

void initBlog(){
    mediumBlog.topicsCount = 0;
    mediumBlog.usersCount = 0;
    for(int i = 0; i < 10; i++){
        mediumBlog.users[i] = false;
    }
}

bool topicAlreadyExists(char *topicName){
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        if(strcmp(mediumBlog.topics[i].name, topicName) == 0) return true;
    }
    return false;
}

bool userAlreadyExists(int userId){
    return mediumBlog.users[userId];
}

void sendNotificationToSubscribers(struct topic topic, char *content){

}

void addPost(int userId, char *content, char *topicName){
    if(topicAlreadyExists(topicName)){
        int topicId = 0;
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            if(strcmp(mediumBlog.topics[i].name, topicName) == 0){
                topicId = i;
                mediumBlog.topics[i].posts[mediumBlog.topics[i].postsCount] = initPost(mediumBlog.topics[i].postsCount, content, userId);
                mediumBlog.topics[i].postsCount++;
            }
        }
        sendNotificationToSubscribers(mediumBlog.topics[topicId], content);
    }
    // create a new topic
    else{
        struct post newPost = initPost(0, content, userId);
        mediumBlog.topics[mediumBlog.topicsCount] = initTopic(mediumBlog.topicsCount, topicName, newPost);
        mediumBlog.topicsCount++;
    }

}

void subscribeToTopic(int userId, struct topic topic){
    if(topicAlreadyExists(topic.name)){
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            if(strcmp(mediumBlog.topics[i].name, topic.name) == 0){
                mediumBlog.topics[i].subscribed[mediumBlog.topics[i].subscribedCount] = true;
                mediumBlog.topics[i].subscribedCount++;
            }
        }
    }
    else{
        //error
    }
}

void unsubcribeToTopic(int userId, struct topic topic){
    if(topicAlreadyExists(topic.name)){
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            if(strcmp(mediumBlog.topics[i].name, topic.name) == 0){
                if(mediumBlog.topics[i].subscribed[userId] == true){
                    mediumBlog.topics[i].subscribed[userId] = false;
                    mediumBlog.topics[i].subscribedCount--;
                }
                else{
                    //error
                }
            }
        }
    }
    else{
        //error
    }
}

int addNewUser(){
    for(int i = 0; i < 10; i++){
        if(mediumBlog.users[i] == false){
            mediumBlog.users[i] = true;
            mediumBlog.usersCount++;
            return i;
        }
    }
}

void removeUser(int userId){
    mediumBlog.users[userId] = false;
    mediumBlog.usersCount--;
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        mediumBlog.topics[i].subscribed[userId] = false;
    }
}

void printBlog(){
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        printf("topic %d: %s\n", i, mediumBlog.topics[i].name);
        for(int j = 0; j < mediumBlog.topics[i].postsCount; j++){
            printf("post %d: %s\n", j, mediumBlog.topics[i].posts[j].content);
        }
    }
}

void initArgs(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: ./server <ip> <port>\n");
        exit(1);
    }
    ip = argv[1];
    port = argv[2];
}

void initSocket(){
    
}

void handleClientRequest(struct BlogOperation request){
    struct BlogOperation response;
  switch (request.operation_type){
  case NEW_CONNECTION:
    printf("client %d connected\n", request.client_id);
    int newClientId = addNewUser();
    printf("new client id: %d\n", newClientId);
    response = initBlogOperation(newClientId, NEW_CONNECTION, "", "");
    break;
  case NEW_POST:
    printf("client %d posted\n", request.client_id);
    break;
  case LIST_TOPICS:
    break;
  case SUBSCRIBE:
    break;
  case UNSUBSCRIBE:
    break;
  case EXIT:
    printf("client %d disconnected\n", request.client_id);
    break;
  default:
    break;
  }

}

int main(int argc, char *argv[]){
    initArgs(argc, argv); // initialize ip and port
    initSocket(); // initialize server's socket
    initBlog(); // initialize blog
    int newClientId = addNewUser();
    addPost(newClientId, "hello world", "topic1");
    printBlog();
}

