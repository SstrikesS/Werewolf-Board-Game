#include "client.h"

//#pragma comment(lib,"ws2_32.lib")

WSADATA wsa;
Argument *getArgument(SOCKET sockfd, struct sockaddr_in server_addr){
    Argument *arg = calloc(1, sizeof(Argument));
    arg->server_addr = server_addr;
    arg->sockfd = sockfd;
    return arg;
}
SOCKET setupSocket(){
    SOCKET sockfd;
	//Initialise winsock
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("[-]Failed. Error Code : %d\n", WSAGetLastError());
		exit(0);
	}
	//Create socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
		printf("[-]Socket creation failed : %d\n" , WSAGetLastError());
		exit(0);
	}
    return sockfd;
}

struct sockaddr_in setupServerAddr(){
    struct sockaddr_in server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDR);
    return server_addr;
}

void sendToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *buffer){
    int length = (int)sizeof(server_addr);
    sendto(sockfd, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, length);
}

void ListenToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *buffer){
    int buffer_size, length = sizeof(server_addr);
    buffer_size = recvfrom(sockfd, buffer, MAX_MESSAGE, 0, (SOCKADDR *)&server_addr, &length);
    buffer[buffer_size] = '\0';
}

int LoginToServer(SOCKET sockfd, struct sockaddr_in server_addr, char *playername){
    int i;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    }
    
    strcpy(token[0], playername);
    buffer = GetMess(token, 1, LOGIN_PACK);

    for(i = 0; i < 2; i++){
        memset(token[i], 0, sizeof(*token[i]));
    }

    sendToServer(sockfd, server_addr, buffer);

    memset(buffer, 0, sizeof(*buffer));

    ListenToServer(sockfd, server_addr, buffer);

    if(strlen(buffer) == 0){
        return ERROR_RETURN;
    }
    token = GetToken(buffer, 3);
    if(token[0] != NULL && strlen(token[0]) > 0){
        enum pack_type type = (enum pack_type)atoi(token[0]);
        printf("[+]Receive message: '%s' from server\n", token[2]);
        if(type == ERROR_PACK){
            printf("[-]Login failed!\n");
            free(buffer);
            free(token);
            return ERROR_RETURN;
        }else if(type == SUCCEED_PACK){
            printf("[+]Login succeed!\n");
        }
    }
    free(buffer);
    free(token);
    return atoi(token[1]);
}

void closeClient(SOCKET sockfd){
    closesocket(sockfd);
	WSACleanup();
}
