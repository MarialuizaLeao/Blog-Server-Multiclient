#include "common.h"

char *ip;
char *port;
int myId = -1;
pthread_t waitingThread;

void initArgs(int argc, char *argv[]);
int parseCommand(char *input);
char *parseContent(int command, char *input);
void handleResponse(struct BlogOperation response);
void* waitForResponse(void* sock);
int initClientSockaddr(const char *ip, const char *portstr, struct sockaddr_storage *storage);
int initSocket();