#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER "26.57.39.175"
#define MAX_CHAR 200	//Max length of buffer
#define PORT 8800	//The port on which to listen for incoming data


//local variable
struct sockaddr_in server_addr;
SOCKET sockfd;
WSADATA wsa;

int setupClient(){
	//Initialise winsock
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		return 1;
	}
	//Create socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){
		printf("Socket creation failed : %d\n" , WSAGetLastError());
		return 1;
	}
    //Setup address structure
    memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER);
    return 0;
}
void closeClient(){
    closesocket(sockfd);
	WSACleanup();
}
int main(){
    int length =sizeof(server_addr);
	char *buffer = (char*)malloc(sizeof(char) * MAX_CHAR);
	char *message = (char*)malloc(sizeof(char) * MAX_CHAR);

	setupClient();
	//Connect to server
	while(1){
		printf("Enter input : ");
		fgets(message, MAX_CHAR, stdin);
		sendto(sockfd, message, strlen(message) , 0 , (SOCKADDR *) &server_addr, length);
		memset(buffer,'\0', MAX_CHAR);
        recvfrom(sockfd, buffer, MAX_CHAR, 0, (SOCKADDR *) &server_addr, &length);
		printf("Server recieve: %s\n", buffer);
	}
    closeClient();
	return 0;
}