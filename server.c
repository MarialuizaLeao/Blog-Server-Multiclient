#include "common.h"

char *ip;
char *port;
struct blog mediumBlog;
sem_t mutex;

struct client
{
    int id;
    int sock;
};

struct client initClient(int id, int sock){
    struct client newClient;
    newClient.id = id;
    newClient.sock = sock;
    return newClient;
}

struct thread{
    struct client client;
    pthread_t thread;
};

struct thread initThread(struct client client){
    struct thread newThread;
    newThread.client = client;
    return newThread;
}

struct thread threads[MAX_USERS];

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
    struct client clients[MAX_USERS];
    bool users[MAX_USERS];
    int topicsCount;
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

void sendNotificationToSubscribers(struct topic topic, int postMadeBy){
    for(int i = 0; i < MAX_USERS; i++){
        if(topic.subscribed[i] == true){
            char* temp = malloc(sizeof(char) * BUFFER_SIZE);
            snprintf(temp, BUFFER_SIZE, "%d", postMadeBy);
            struct BlogOperation notification = initBlogOperation(i, NEW_POST, topic.name, temp, -1);
            size_t count_bytes_sent = send(mediumBlog.clients[i].sock, &notification, sizeof(struct BlogOperation), 0);
            if(count_bytes_sent != sizeof(struct BlogOperation)) logexit("send");
        }
    }
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
        sendNotificationToSubscribers(mediumBlog.topics[topicId], userId + 1);
    }
    else{
        struct post newPost = initPost(0, content, userId);
        mediumBlog.topics[mediumBlog.topicsCount] = initTopic(mediumBlog.topicsCount, topicName, newPost);
        mediumBlog.topicsCount++;
    }

}

void subscribeToTopic(int userId, char *topic){
    if(topicAlreadyExists(topic)){
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            if(strcmp(mediumBlog.topics[i].name, topic) == 0){
                mediumBlog.topics[i].subscribed[userId] = true;
                mediumBlog.topics[i].subscribedCount++;
            }
        }
    }
    else{
        //error
    }
}

void unsubcribeToTopic(int userId, char* topic){
    if(topicAlreadyExists(topic)){
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            if(strcmp(mediumBlog.topics[i].name, topic) == 0){
                printf("subscribedCount: %d\n", mediumBlog.topics[i].subscribedCount);
                if(mediumBlog.topics[i].subscribed[userId] == true){
                    mediumBlog.topics[i].subscribed[userId] = false;
                    mediumBlog.topics[i].subscribedCount--;
                    printf("subscribedCount: %d\n", mediumBlog.topics[i].subscribedCount);
                }
                else{
                    //ERROR
                }
            }
        }
    }
    else{
        //error
    }
}

int addNewUser(int sock){
    sem_wait(&mutex);
    for(int i = 0; i < 10; i++){
        if(mediumBlog.users[i] == false){
            mediumBlog.users[i] = true;
            mediumBlog.usersCount++;
            mediumBlog.clients[i] = initClient(i, sock);
            sem_post(&mutex);
            return i;
        }
    }
    sem_post(&mutex);
    return -1;
}

void removeUser(int userId){
    sem_wait(&mutex);
    mediumBlog.users[userId] = false;
    mediumBlog.usersCount--;
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        if(mediumBlog.topics[i].subscribed[userId] == true){
            mediumBlog.topics[i].subscribed[userId] = false;
        }
    }
    sem_post(&mutex);
}

void getTopics(char *topics){
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        strcat(topics, mediumBlog.topics[i].name);
        if(i != mediumBlog.topicsCount - 1) strcat(topics, "; ");
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

struct BlogOperation handleClientRequest(struct BlogOperation request){
    struct BlogOperation response = initBlogOperation(request.client_id, 0, "", "", -1);
    switch (request.operation_type){
        case NEW_POST:
            addPost(request.client_id, request.content, request.topic);
            printf("new post added in %s by 0%d\n", request.topic, request.client_id+1);
            break;
        case LIST_TOPICS:
            printf("client 0%d requested topics\n", request.client_id+1);
            char *topics = malloc(sizeof(char) * BUFFER_SIZE);
            getTopics(topics);
            response = initBlogOperation(request.client_id, LIST_TOPICS, "", topics, 1);
            break;
        case SUBSCRIBE:
            subscribeToTopic(request.client_id, request.topic);
            printf("client 0%d subscribed to %s\n", request.client_id+1, request.topic);
            break;
        case UNSUBSCRIBE:
            unsubcribeToTopic(request.client_id, request.topic);
            printf("client 0%d unsubscribed to %s\n", request.client_id+1, request.topic);
            break;
        case EXIT:
            removeUser(request.client_id);
            printf("client %d was disconnected\n", request.client_id);
            break;
        default:
            printf("Entrou no caso default\n");
            break;
    }
    return response;
}

void* clientFunction(void* clientThread)
{
    struct thread *clientThreadPtr = (struct thread*) clientThread;
    int sockfd = clientThreadPtr->client.sock;
    struct BlogOperation request;
    while(true){
        receive_all(sockfd, &request, sizeof(struct BlogOperation));
        printf("client 0%d sent %d\n", request.client_id + 1, request.operation_type);
        // handle client request
        struct BlogOperation response = handleClientRequest(request);
        
        // send response to client
        if(response.server_response != -1){
            size_t count_bytes_sent = send(sockfd, &response, sizeof(struct BlogOperation), 0);
            if(count_bytes_sent != sizeof(struct BlogOperation)) logexit("send");
        }
        // if client wants to exit, close connection
        if(request.operation_type == EXIT){
            close(sockfd);
            break;
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

int serverSockaddrInit(const char *ipProtocol, const char *portstr, struct sockaddr_storage *storage){
    uint16_t port = (uint16_t) atoi(portstr);
    if(port == 0) return -1;
    port = htons(port);

    memset(storage, 0, sizeof(*storage));

    if(0 == strcmp(ipProtocol, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = htonl(INADDR_ANY); //Qualquer add disponivel no computador
        return 0;
    }
    else if(0 == strcmp(ipProtocol, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any; //Qualquer add disponivel no computador v6
        return 0;
    }
    else return -1;
}

int socketInit(){
    // initialize address
    struct sockaddr_storage storage;
    if(serverSockaddrInit(ip, port, &storage)) logexit("serverSockaddrInitt");
    // initialize socket
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1) logexit("socket");
    // reuse address
    int enable = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) logexit("setsockopt");
    // bind to address
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(sockfd, addr, sizeof(storage)) != 0) logexit("bind");
    // listen for connections
    if(listen(sockfd, 10) != 0) logexit("listen");
    return sockfd;
}

int main(int argc, char *argv[]){
    initArgs(argc, argv); // initialize ip and port
    sem_init(&mutex, 0, 1);
    initBlog(); // initialize blog
    int sockfd = socketInit(); // initialize socket
    struct thread clientsThreads[MAX_USERS];

    while(true){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(sockfd, caddr, &caddrlen); // complete connection between server and client
        if(csock == -1) logexit("accept");

        int newClientId = addNewUser(csock);
        threads[newClientId] = initThread(initClient(newClientId, csock));

        printf("client 0%d connected\n", newClientId + 1);

        struct BlogOperation response = initBlogOperation(newClientId, NEW_CONNECTION, "", "", 1);
        size_t count = send(csock, &response, sizeof(struct BlogOperation), 0);
        if(count != sizeof(struct BlogOperation)) logexit("send");

        if(pthread_create(&(threads[newClientId].thread), NULL, clientFunction,(void*) &threads[newClientId]) != 0) logexit("pthread_create");

    }

    for(int i = 0; i < MAX_USERS; i++){
        pthread_join(threads[i].thread, NULL);
    }
}

