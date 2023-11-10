#include "post.c"

struct topic
{
    int id;
    char name[50];
    bool subscribed[MAX_USERS];
    int subscribedCount;
    struct post posts[MAX_LEN_TOPIC];
    int postsCount;
};

struct topic initTopic(int id, char name, struct post newPost)
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