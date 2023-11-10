#include "common.h"

char *ip;
char *port;

int myID = 0;

void initArgs(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: ./client <ip> <port>\n");
        exit(1);
    }
    ip = argv[1];
    port = argv[2];
}

void initSocket(){

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
        char *inKeyword = strtok(NULL, " ");
        if(inKeyword == NULL || !(strcmp(inKeyword, "in") == 0 || strcmp(inKeyword, "to") == 0)){
            return -1;
        }
        return SUBSCRIBE;
    }
    if(strcmp(command, "unsubscribe") == 0){
        char *inKeyword = strtok(NULL, " ");
        if(inKeyword == NULL || !(strcmp(inKeyword, "in") == 0 || strcmp(inKeyword, "to") == 0)){
            return -1;
        }
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
    if(command == NEW_POST){
        return input + 11;
    }
    else if(command == SUBSCRIBE){
        return input + 11;
    }
    else if(command == UNSUBSCRIBE){
        return input + 12;
    }
    
}

void handleResponse(struct BlogOperation response){
    if(response.operation_type == NEW_CONNECTION){
        myID = response.client_id;
        printf("client %d connected\n", response.client_id);
    }
    else if(response.operation_type == NEW_POST){
        printf("new post added in %s by %d\n", response.topic, response.client_id);
    }
    else if(response.operation_type == LIST_TOPICS){
        printf("list topics\n");
    }
    else if(response.operation_type == SUBSCRIBE){
        printf("subscribed to %s\n", response.topic);
    }
    else if(response.operation_type == UNSUBSCRIBE){
        printf("unsubscribed from %s\n", response.topic);
    }
    else if(response.operation_type == EXIT){
        printf("exit\n");
    }
    else{
        printf("error\n");
    }
}

int main(int argc, char **argv){
    initArgs(argc, argv);
    initSocket();
    char input[BUFFER_SIZE];
    while(true){
        struct BlogOperation request;
        fgets(input, BUFFER_SIZE, stdin);
        int cmdType = parseCommand(input);
        if(cmdType == NEW_CONNECTION){
            request = initBlogOperation(0, cmdType, "", "");
        }
        else if(cmdType == NEW_POST){
            char *topic = parseContent(cmdType, input);
            fgets(input, BUFFER_SIZE, stdin);
            request = initBlogOperation(myID, cmdType, topic, input);
            // create a new post
        }
        else if(cmdType == LIST_TOPICS){
            request = initBlogOperation(myID, cmdType, "", "");
            // list all topics
        }
        else if(cmdType == SUBSCRIBE){
            char *topic = parseContent(cmdType, input);
            request = initBlogOperation(myID, cmdType, topic, "");
            // subscribe to a topic
        }
        else if(cmdType == UNSUBSCRIBE){
            char *topic = parseContent(cmdType, input);
            request = initBlogOperation(myID, cmdType, topic, "");
            // unsubscribe from a topic
        }
        else if(cmdType == EXIT){
            printf("exit\n");
            request = initBlogOperation(myID, cmdType, "", "");
            // exit
        }
        else{
            printf("error\n");
            // error
        }
        //send request
        //receive response
    }
}
