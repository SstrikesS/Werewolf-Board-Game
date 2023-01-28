#include <winsock2.h>
#include "packet.h"
#include "linklist.h"
#include <pthread.h>
#include "player.h"

#pragma comment(lib,"ws2_32.lib") 

#define MAX_CLIENT 100 //Max clients can connect to server

// local variable
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;
Client *client_list;
Room *room_list;
int thread_count = 0;

typedef struct Argument{
	int index;
	enum pack_type type;
	char* message;
}Argument;

int setupServer(){ 
    WSADATA wsa;
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

int addClient(char *client_name){
	int i;
	for(i = 0; i < MAX_CLIENT; i++){
		if(client_list[i].id == -1){
			client_list[i].id = i + 1;
			strcpy(client_list[i].name, client_name);
			client_list[i].client = client_addr;
			printf("%s with id %d from ip %s is joined server!\n", client_list[i].name, client_list[i].id, inet_ntoa(client_list[i].client.sin_addr));
			return SUCCEED_RETURN;
		}
	}
	return ERROR_RETURN;
}

void *client_handle(void *argument){
	pthread_detach(pthread_self());
	Argument *arg = (Argument*)argument;
	char *message = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	struct sockaddr_in client_addr = client_list[arg->index].client;
	
	int length = sizeof(client_addr);
	switch (arg->type){
	// case PUBLIC_MESSAGE_PACK:
	// 	packet = MessageParse(arg->message);
	// 	printf("Received packet from %s port %d: %s\n", inet_ntoa(arg->current_client->client.sin_addr), ntohs(arg->current_client->client.sin_port), arg->message);
	// 	message = GetPackMessage_PubMess(packet,arg->current_client->name);
	// 	while(current != NULL){
	// 		Client *current_client = (Client *)current->data;
	// 		sendto(sockfd, message, strlen(message), 0, (SOCKADDR *) &current_client->client, length);
	// 		current = current->next;
	// 	}
	// 	break;
	case ROOM_PACK:
		break;
	case EXIT_PACK:
		printf("Exit packet received!\n");
		break;
	default:
		printf("Can't recognize packet %s!\n", arg->message);
		break;
	}
}

void checkLogin(){
	int length, buffer_size, check, i;
	enum pack_type type;
	char *buffer = calloc( MAX_OF_CHAR_MESSAGE, sizeof(char));
	char **token = calloc(10, sizeof(char*));
	for(i = 0; i < 10; i++){
		token[i] = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
	}
	length = sizeof(server_addr) ;
	pthread_t tid[MAX_CLIENT];

	if(setupServer() == SUCCEED_RETURN){
        printf("Server is now online...\n");
        while(1){
			memset(&client_addr, 0, sizeof(client_addr));
            memset(buffer, '\0', MAX_OF_CHAR_MESSAGE);
			ZeroMemory(token, sizeof(token));
			for(i = 0; i < 10; i++){
				memset(token[i], '\0', MAX_OF_CHAR_MESSAGE);
			}
            buffer_size = recvfrom(sockfd, buffer, MAX_OF_CHAR_MESSAGE, 0, (SOCKADDR *)&client_addr, &length);
			buffer[buffer_size] = '\0';
			type = GetMessageType(buffer);
			if(type == LOGIN_PACK){
				token = GetToken(buffer, 2);
				check = addClient(token[1]);
				if(check == SUCCEED_RETURN){
					memset(buffer, '\0', MAX_OF_CHAR_MESSAGE);
					buffer = GetMessage_SUCCESS("Welcome to our server");
					sendto(sockfd, buffer, strlen(buffer), 0, (SOCKADDR *) &client_addr, length);
				}
			}else{
				for(i = 0; i < MAX_CLIENT; i++){
					if((client_list[i].client.sin_addr.s_addr == client_addr.sin_addr.s_addr) && (client_list[i].client.sin_port == client_addr.sin_port)){
						Argument *arg = calloc(1, sizeof(Argument));
						arg->index = i;
						arg->message = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
						memset(arg->message, '\0', MAX_OF_CHAR_MESSAGE);
						strcpy(arg->message, buffer);
						arg->type = type;
						pthread_create(&tid[thread_count++], NULL, &client_handle, (void *)arg);
					}
				}
			}
        }
    }
}

int main(){
	room_list = calloc(MAX_OF_ROOM, sizeof(Room));
	client_list = calloc(MAX_CLIENT, sizeof(Client));
	int i;
	for(i = 0; i < MAX_OF_ROOM; i++){
		room_list[i].room_id = calloc(MAX_OF_NAME, sizeof(char));
		room_list[i].client_curr = 0;
	}
	for(i = 0; i < MAX_OF_ROOM; i++){
		client_list[i].host = -1;
		client_list[i].id = -1;
		client_list[i].name = calloc(MAX_OF_NAME, sizeof(char));
		memset(&client_list[i].client, 0, sizeof(client_list[i].client));
		client_list[i].room_id = calloc(MAX_OF_NAME, sizeof(char)); 
	}
	checkLogin();
	closeServer();
	return 0;
}