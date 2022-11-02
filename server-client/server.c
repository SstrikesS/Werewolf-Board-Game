#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") 

#define MAX_NAME_CHAR 20
#define MAX_CHAR 200	//Max length of buffer
#define PORT 8800	//The port on which to listen for incoming data
#define MAX_CLIENT 12 //Max clients can connect to server

// local variable
int count_client = 0;
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr[MAX_CLIENT];
WSADATA wsa;

// client struct data
typedef struct ClientInfo{
    char name[MAX_NAME_CHAR];
    int id;
    struct sockaddr_in client;
}ClientInfo;

ClientInfo getClientName(struct sockaddr_in client, char *name){
    ClientInfo tmp;
    strcpy(tmp.name, name);
    tmp.id = ++count_client;
    tmp.client = client;
    return tmp;
}


int setupServer(){ 
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		return 1;
	}
	//Create a socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		printf("Could not create socket : %d\n" , WSAGetLastError());
        return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("26.57.39.175");
	server_addr.sin_port = htons(PORT);
	
	//Bind
	if( bind(sockfd ,(SOCKADDR *)&server_addr , sizeof(server_addr)) == SOCKET_ERROR){
		printf("Bind failed with error code : %d\n" , WSAGetLastError());
		return 1;
	}
    return 0; // succeed create server
}

void closeServer(){
    closesocket(sockfd);
	WSACleanup();
}

int main(){
	int length, recv;
	char *buffer = (char *)malloc(sizeof(char) *MAX_CHAR);
	length = sizeof(server_addr) ;
	
    if(setupServer() == 0){
        printf("Server is now online...\n");
        while(1){
            // server do
            fflush(stdout);
            memset(buffer,'\0', MAX_CHAR);
            recv = recvfrom(sockfd, buffer, MAX_CHAR, 0, (SOCKADDR *) &client_addr[0], &length);
            printf("Received packet from %s port %d: %s\n", inet_ntoa(client_addr[0].sin_addr), ntohs(client_addr[0].sin_port), buffer);
            
            sendto(sockfd, buffer, recv, 0, (SOCKADDR *) &client_addr[0], length);
        }
    }
	closeServer();
	return 0;
}