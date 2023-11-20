#include "server.h"


int main(int argc, char *argv[]){
    initArgs(argc, argv); // initialize ip and port
    initBlog(); // initialize blog
    int sockfd = initSocket(); // initialize socket

    while(true){
        // accept connection
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(sockfd, caddr, &caddrlen); // complete connection between server and client
        if(csock == -1) logexit("accept");
        // add new user to the blog
        int newClientId = addNewUser(csock);
        printf("client %02d connected\n", newClientId + 1);
        // send new connection response to client
        struct BlogOperation response = initBlogOperation(newClientId, NEW_CONNECTION, "", "", 1);
        size_t count = send(csock, &response, sizeof(struct BlogOperation), 0);
        if(count != sizeof(struct BlogOperation)) logexit("send");
        // create new thread for client
        if(pthread_create(&(mediumBlog.clients[newClientId].thread), NULL, clientFunction,(void*) &mediumBlog.clients[newClientId]) != 0) logexit("pthread_create");
    }

    for(int i = 0; i < MAX_USERS; i++){
        pthread_join(mediumBlog.clients[i].thread, NULL);
    }
}

struct Client initClient(int id, int sock){
    struct Client newClient;
    newClient.id = id;
    newClient.sock = sock;
    return newClient;
}

struct Post initPost(int id, char *content, int madeBy){
    struct Post newPost;
    newPost.id = id;
    strcpy(newPost.content, content);
    newPost.madeBy = madeBy;
    return newPost;
}

struct Topic initTopic(int id, char *name){
    struct Topic newTopic;

    newTopic.id = id;
    strcpy(newTopic.name, name);
    for(int i = 0; i < 10; i++){
        newTopic.subscribed[i] = false;
    }
    newTopic.subscribedCount = 0;
    newTopic.postsCount = 0;
    return newTopic;
}

void initBlog(){
    mediumBlog.topicsCount = 0;
    mediumBlog.usersCount = 0;
    for(int i = 0; i < 10; i++){
        mediumBlog.users[i] = false;
    }
}

// Check if topic already exists in the blog. If it does, the function will return its ID. If it doesn't, the function will return -1.
int topicAlreadyExists(char *topicName){
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        if(strcmp(mediumBlog.topics[i].name, topicName) == 0) return i;
    }
    return -1;
}

// Send notification to all subscribers of a topic
void sendNotificationToSubscribers(struct Topic topic, int postMadeBy, char *content){
    for(int i = 0; i < MAX_USERS; i++){
        if(topic.subscribed[i] == true){
            struct BlogOperation notification = initBlogOperation(postMadeBy, NEW_POST, topic.name, content, -1);
            size_t count_bytes_sent = send(mediumBlog.clients[i].sock, &notification, sizeof(struct BlogOperation), 0);
            if(count_bytes_sent != sizeof(struct BlogOperation)) logexit("send");
        }
    }
}

// Add a new post to a topic
void addPost(int userId, char *content, char *topicName){
    int topicId = topicAlreadyExists(topicName);
    if(topicId != -1){
        mediumBlog.topics[topicId].posts[mediumBlog.topics[topicId].postsCount] = initPost(mediumBlog.topics[topicId].postsCount, content, userId);
        mediumBlog.topics[topicId].postsCount++;
        sendNotificationToSubscribers(mediumBlog.topics[topicId], userId + 1, content);
    }
    else{
        struct Post newPost = initPost(0, content, userId);
        mediumBlog.topics[mediumBlog.topicsCount] = initTopic(mediumBlog.topicsCount, topicName);
        mediumBlog.topics[mediumBlog.topicsCount].posts[0] = newPost;
        mediumBlog.topics[mediumBlog.topicsCount].postsCount++;
        mediumBlog.topicsCount++;
    }
}

// Subscribe to a topic
int subscribeToTopic(int userId, char *topicName){
    int topicId = topicAlreadyExists(topicName);
    if(topicId != -1){
        if(mediumBlog.topics[topicId].subscribed[userId] == false){
            mediumBlog.topics[topicId].subscribed[userId] = true;
            mediumBlog.topics[topicId].subscribedCount++;
        }
        else{
            return ERROR_ALREADY_SUBSCRIBED;
        }
    }
    else{
        // create new topic
        mediumBlog.topics[mediumBlog.topicsCount] = initTopic(mediumBlog.topicsCount, topicName);
        mediumBlog.topics[mediumBlog.topicsCount].subscribed[userId] = true;
        mediumBlog.topics[mediumBlog.topicsCount].subscribedCount++;
        mediumBlog.topicsCount++;
    }
    return SUCCESS;
}

// Unsubscribe to a topic
int unsubscribeToTopic(int userId, char* topic){
    int topicId = topicAlreadyExists(topic);
    if(topicId != -1){
        if(mediumBlog.topics[topicId].subscribed[userId] == true){
            mediumBlog.topics[topicId].subscribed[userId] = false;
            mediumBlog.topics[topicId].subscribedCount--;
            return SUCCESS;
        }
        else{
            return ERROR_NOT_SUBSCRIBED;
        }
    }
    else{
        return ERROR_TOPIC_NOT_FOUND;
    }
}

// Add a new user to the blog
int addNewUser(int sock){
    for(int i = 0; i < 10; i++){
        if(mediumBlog.users[i] == false){
            mediumBlog.users[i] = true;
            mediumBlog.usersCount++;
            mediumBlog.clients[i] = initClient(i, sock);
            return i;
        }
    }
    return -1;
}

// Remove a user from the blog
void removeUser(int userId){
    mediumBlog.users[userId] = false;
    mediumBlog.usersCount--;
    for(int i = 0; i < mediumBlog.topicsCount; i++){
        if(mediumBlog.topics[i].subscribed[userId] == true){
            mediumBlog.topics[i].subscribed[userId] = false;
        }
    }
}

// Initialize a blog operation
void getTopics(char *topics){
    if(mediumBlog.topicsCount != 0){
        for(int i = 0; i < mediumBlog.topicsCount; i++){
            strcat(topics, mediumBlog.topics[i].name);
            if(i != mediumBlog.topicsCount - 1) strcat(topics, ";");
        }
    }
    else{
        strcpy(topics, "no topics available");
    }
}

// Initialize the arguments of the server
void initArgs(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: ./server <ip> <port>\n");
        exit(1);
    }
    ip = argv[1];
    port = argv[2];
}

// Receives the client request and returns the aproppriate response
struct BlogOperation handleClientRequest(struct BlogOperation request){
    struct BlogOperation response = initBlogOperation(request.client_id, 0, "", "", -1);
    switch (request.operation_type){
        case NEW_POST:
            addPost(request.client_id, request.content, request.topic);
            printf("new post added in %s by %02d\n", request.topic, request.client_id+1);
            break;
        case LIST_TOPICS:;
            char *topics = malloc(sizeof(char) * BUFFER_SIZE);
            getTopics(topics);
            response = initBlogOperation(request.client_id, LIST_TOPICS, "", topics, 1);
            break;
        case SUBSCRIBE:
            if(subscribeToTopic(request.client_id, request.topic) != SUCCESS) response = initBlogOperation(request.client_id, ERROR, "", "error: already subscribed", 1);
            else printf("client %02d subscribed to %s\n", request.client_id+1, request.topic);
            break;
        case UNSUBSCRIBE:
            if(unsubscribeToTopic(request.client_id, request.topic) != SUCCESS) response = initBlogOperation(request.client_id, ERROR, "", "error: not subscribed", 1);
            else printf("client %02d unsubscribed to %s\n", request.client_id+1, request.topic);
            break;
        case EXIT:
            removeUser(request.client_id);
            printf("client %02d disconnected\n", request.client_id + 1);
            break;
        default:
            break;
    }
    return response;
}

// Function for handling the client thread
void* clientFunction(void* clientThread)
{
    struct Client *clientThreadPtr = (struct Client*) clientThread;
    int sockfd = clientThreadPtr->sock;
    struct BlogOperation request;
    while(true){
        // receive request from client
        int count = recv(sockfd, &request, sizeof(struct BlogOperation), 0);

        // if client disconnected, close connection
        if(count == 0){
            request.operation_type = EXIT;
        }
        else if(count != sizeof(struct BlogOperation)) logexit("recv");

        // handle client request
        struct BlogOperation response = handleClientRequest(request);
        
        // send response to client
        if(response.server_response != ERROR){
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

// Initialize the socket address
int initServerSockaddr(const char *ipProtocol, const char *portstr, struct sockaddr_storage *storage){
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

// Initialize the socket
int initSocket(){
    // initialize address
    struct sockaddr_storage storage;
    if(initServerSockaddr(ip, port, &storage)) logexit("serverSockaddrInitt");
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
