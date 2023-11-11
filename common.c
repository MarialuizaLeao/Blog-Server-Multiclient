#include "common.h"

struct BlogOperation initBlogOperation(int client_id, int operation_type, char *topic, char *content, int server_response){
    struct BlogOperation blogOperation;
    blogOperation.client_id = client_id;
    blogOperation.operation_type = operation_type;
    blogOperation.server_response = 0;
    strcpy(blogOperation.topic, topic);
    strcpy(blogOperation.content, content);
    return blogOperation;
}

void logexit(const char *str) {
  perror(str);
  exit(EXIT_FAILURE);
}

// Parses the address and port from the command line arguments
int addrparse(const char * addrstr, const char * portstr, struct sockaddr_storage * storage) {
  if (addrstr == NULL || portstr == NULL) {
    return -1;
  }

  uint16_t port = (uint16_t) atoi(portstr);
  
  if (port == 0) {
    return -1;
  }

  port = htons(port);

  struct in_addr inaddr4;
  if (inet_pton(AF_INET, addrstr, &inaddr4)) {
    struct sockaddr_in * addr4 = (struct sockaddr_in *) storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr = inaddr4;
    return 0;
  }

  struct in6_addr inaddr6;
  if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
    struct sockaddr_in6 * addr6 = (struct sockaddr_in6 *) storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
    return 0;
  }

  return -1;
}

// Initializes the socket address
int server_sockaddr_init(const char * protocol_version, const char * portstr, struct sockaddr_storage * storage) {
  uint16_t port = (uint16_t) atoi(portstr);
  if (port == 0) {
    return -1;
  }

  port = htons(port);

  memset(storage, 0, sizeof(*storage));
  if (strcmp(protocol_version, "v4") == 0) {
    struct sockaddr_in * addr4 = (struct sockaddr_in *) storage;
    addr4->sin_family = AF_INET;
    addr4->sin_addr.s_addr = INADDR_ANY;
    addr4->sin_port = port;
  } else if (strcmp(protocol_version, "v6") == 0) {
    struct sockaddr_in6 * addr6 = (struct sockaddr_in6 *) storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_addr = in6addr_any;
    addr6->sin6_port = port;
  } else {
    return -1;
  }

  return 0;
}

size_t receive_all(int socket, void *buffer, size_t size)
{
    size_t total_received = 0;
    while(total_received < size){
        size_t bytes_received = recv(socket, buffer + total_received, size - total_received, 0);
        if(bytes_received <= 0){
            if(bytes_received == 0)
                return total_received;
            else{
                perror("Error receiving data");
                return -1;
            }
        }
        total_received += bytes_received;
    }
    return total_received;
}