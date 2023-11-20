#include "client.h"

int main(int argc, char **argv){
    initArgs(argc, argv);
    int sockfd = initSocket();
    struct BlogOperation request = initBlogOperation(0, NEW_CONNECTION, "", "", 0);
    int count = send(sockfd, &request, sizeof(request), 0); // send request to server
    if(count != sizeof(request)) logexit("send");
    struct BlogOperation response;
    int count_bytes_recv = recv(sockfd, &response, sizeof(struct BlogOperation), 0); // receive response from server
    if(count_bytes_recv != sizeof(struct BlogOperation)) logexit("recv");
    myId = response.client_id;
    pthread_create(&waitingThread, NULL, &waitForResponse, (void*) &sockfd);

    char input[BUFFER_SIZE];
    while(true){
        fgets(input, BUFFER_SIZE, stdin);
        int cmdType = parseCommand(input);
        char *topic = " ";
        switch(cmdType){
            case  NEW_POST:
                topic = parseContent(cmdType, input);
                fgets(input, BUFFER_SIZE, stdin);
                char content[BUFFER_SIZE];
                strcpy(content, input);
                request = initBlogOperation(myId, cmdType, topic, content, 0);
                break;
            case LIST_TOPICS:
                request = initBlogOperation(myId, cmdType, "", "", 0);
                break;
            case SUBSCRIBE:
                topic = parseContent(cmdType, input);
                request = initBlogOperation(myId, cmdType, topic, "", 0);
                break;
            case UNSUBSCRIBE:
                topic = parseContent(cmdType, input);
                request = initBlogOperation(myId, cmdType, topic, "", 0);
                break;
            case EXIT:
                request = initBlogOperation(myId, cmdType, "", "", 0);
                break;
            case ERROR:
                printf("Invalid command\n");
                continue;
                break;
            default:
                break;
        }
        if(cmdType != ERROR){
            size_t count_bytes_sent = send(sockfd, &request, sizeof(request), 0);
            if(count_bytes_sent != sizeof(struct BlogOperation)) logexit("send");
        }
        if(cmdType == EXIT) break;
    }
}

void initArgs(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: ./client <ip> <port>\n");
        exit(1);
    }
    ip = argv[1];
    port = argv[2];
}

int parseCommand(char *input){
    input[strlen(input) - 1] = '\0';
    char *temp = malloc(sizeof(char) * BUFFER_SIZE);
    strcpy(temp, input);
    char *command = strtok(temp, " ");

    if(strcmp(input, "exit") == 0){
        return EXIT;
    }
    if(strcmp(input, "list topics") == 0){
        return LIST_TOPICS;
    }
    
    if(strcmp(command, "subscribe") == 0){
        return SUBSCRIBE;
    }
    if(strcmp(command, "unsubscribe") == 0){
        return UNSUBSCRIBE;
    }
    if(strcmp(command, "publish") == 0){
        char *inKeyword = strtok(NULL, " ");
        if(inKeyword == NULL || strcmp(inKeyword, "in") != 0){
            return -1;
        }
        return NEW_POST;
    }
    return -1;
}

char *parseContent(int command, char *input){
    char *temp = malloc(sizeof(char) * BUFFER_SIZE);
    if(command == NEW_POST){
        strcpy(temp, input);
        return temp + 11;
    }
    else if(command == SUBSCRIBE){
        char *inKeyword = strtok(NULL, " ");
        if(inKeyword == NULL || !(strcmp(inKeyword, "in") == 0 || strcmp(inKeyword, "to") == 0)){
            return input + 10;
        }
        return input + 13;
    }
    else if(command == UNSUBSCRIBE){
        char *inKeyword = strtok(NULL, " ");
        if(inKeyword == NULL || !(strcmp(inKeyword, "in") == 0 || strcmp(inKeyword, "to") == 0)){
            return input + 12;
        }
        return input + 15;
    }
    return NULL;
}

void handleResponse(struct BlogOperation response){
    if(response.operation_type == LIST_TOPICS){
        printf("%s\n", response.content);
    }
    else if(response.operation_type == NEW_POST){
        printf("new post added in %s by %02d\n%s", response.topic, response.client_id, response.content);
    }
    else if(response.operation_type == ERROR){
        printf("%s\n", response.content);
    }
}

void* waitForResponse(void* sock){
    int* sockfd = (int*) sock;
    while(true){
        struct BlogOperation response;
        int count = recv(*sockfd, &response, sizeof(struct BlogOperation), 0);
        if(count != sizeof(struct BlogOperation)) logexit("recv");
        handleResponse(response);
        if(response.operation_type == EXIT){
            close(*sockfd);
            break;
        }
    }
    pthread_exit(NULL);
}

int initClientSockaddr(const char *ip, const char *portstr, struct sockaddr_storage *storage){
    if(ip == NULL || portstr == NULL) return -1;
    // inicialize port
    uint16_t port = (uint16_t) atoi(portstr);
    if(port == 0) return -1;
    port = htons(port); //host to network (litle endian)
    // inicialize ip
    struct in_addr inaddr4; //32 bit IP Address
    if(inet_pton(AF_INET, ip, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }
    struct in6_addr inaddr6; //128 bit IP Address
    if(inet_pton(AF_INET6, ip, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6 -> sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }
    return -1;
}

int initSocket(){
    // inicialize address
    struct sockaddr_storage storage;
    if(initClientSockaddr(ip, port, &storage) != 0) logexit("clientSockaddrInit");
    // inicialize socket
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1) logexit("socket");
    // inicialize connection
    struct sockaddr *addr = (struct sockaddr *)(&storage); 
    if(connect(sockfd, addr, sizeof(storage)) != 0) logexit("connect");
    return sockfd;
}