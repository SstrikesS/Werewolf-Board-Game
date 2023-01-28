#include <winsock2.h>
#include "globaldefine.h"
#include "pack.h"
#include <pthread.h>

typedef struct _Argument{
    struct sockaddr_in server_addr;
    char **token;
    SOCKET sockfd;
}Argument;

SOCKET setupSocket();

struct sockaddr_in setupServerAddr();

void closeClient(SOCKET sockfd);

int LoginToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *playername);

void sendToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *buffer);

void ListenToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *buffer);