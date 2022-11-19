#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <unistd.h>
#include "packet.h"
#pragma comment(lib, "ws2_32.lib")

//local variable
struct sockaddr_in server_addr;
SOCKET sockfd;
WSADATA wsa;

int setupClient(){
	//Initialise winsock
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		return ERROR_RETURN;
	}
	//Create socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
		printf("Socket creation failed : %d\n" , WSAGetLastError());
		return ERROR_RETURN;
	}
    //Setup address structure
    memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDR);
    return SUCCEED_RETURN;
}

void closeClient(){
    closesocket(sockfd);
	WSACleanup();
}

char * getClient_Info(){
	char *name_client = (char*)malloc(sizeof(char) *MAX_OF_NAME);
	do{
		printf("Enter your name (Your name must have atleast 4 characters): ");
		fflush(stdin);
		scanf("%[^\n]%*c", name_client);
	}while(strlen(name_client) < 4);
}
int main(){
    int length =sizeof(server_addr);
	char *buffer = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	char *message = (char*)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	Packet *pack = (Packet *)malloc(sizeof(Packet));
	int buffer_size;
	if(setupClient() == SUCCEED_RETURN){
		//Connect to server
		while(1){
			memset(message, '\0', MAX_OF_CHAR_MESSAGE);
			ZeroMemory(pack , sizeof(pack));
			printf("Enter input : ");
			fflush(stdin);
			scanf("%[^\n]%*c",pack->message);
			if(strlen(pack->message) == 0){
				printf("Exit client\n");
				pack->type = EXIT_PACK;
				message = PacketParse(pack);
				sendto(sockfd, message, strlen(message), 0, (SOCKADDR *) &server_addr, length);
				break;
			}else{
				pack->type = PUBLIC_MESSAGE_PACK;
				message = PacketParse(pack);
				sendto(sockfd, message, strlen(message) , 0, (SOCKADDR *) &server_addr, length);
				memset(buffer, '\0', MAX_OF_CHAR_MESSAGE);
				buffer_size = recvfrom(sockfd, buffer, MAX_OF_CHAR_MESSAGE, 0, (SOCKADDR *) &server_addr, &length);
				buffer[buffer_size] = '\0';
				printf("Server recieve: %s\n", buffer);
			}
		}
	}
    closeClient();
	return 0;
}