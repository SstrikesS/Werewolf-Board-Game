#include <stdio.h>
#include <winsock2.h>
#include "linklist.h"
#include "packet.h"

#pragma comment(lib,"ws2_32.lib") 

#define MAX_CLIENT 12 //Max clients can connect to server

typedef struct Player{
	struct sockaddr_in player_client;
	char *name;
	int id;
}Player;
// local variable
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;

int setupServer(){ 
    WSADATA wsa;
	unsigned int length = sizeof(client_addr);
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		return ERROR_RETURN;
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		printf("Could not create socket : %d\n" , WSAGetLastError());
        return ERROR_RETURN;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("26.57.39.175");
	server_addr.sin_port = htons(PORT);
	
	if(bind(sockfd ,(SOCKADDR *)&server_addr , sizeof(server_addr)) == SOCKET_ERROR){
		printf("Bind failed with error code : %d\n" , WSAGetLastError());
		return ERROR_RETURN;
	}
    return SUCCEED_RETURN;
}

void closeServer(){
    closesocket(sockfd);
	WSACleanup();
}


int main(){
	int length, buffer_size;
	char *buffer = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	length = sizeof(server_addr) ;
	Packet *pack;
    if(setupServer() == SUCCEED_RETURN){
        printf("Server is now online...\n");
        while(1){
			printf("Waiting for request...\n");
            memset(buffer, '\0', MAX_OF_CHAR_MESSAGE);
			memset(pack, 0, sizeof(pack));
            buffer_size = recvfrom(sockfd, buffer, MAX_OF_CHAR_MESSAGE, 0, (SOCKADDR *) &client_addr, &length);
			buffer[buffer_size] = '\0';
			pack = MessageParse(buffer);
			if(pack->type == EXIT_PACK){
				printf("Exit packet received!\n");
				break;
			}else if(pack->type == PUBLIC_MESSAGE_PACK){
				printf("Received packet from %s port %d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), pack->message);
            	sendto(sockfd, pack->message, strlen(pack->message), 0, (SOCKADDR *) &client_addr, length);
			}
        }
    }
	closeServer();
	return 0;
}