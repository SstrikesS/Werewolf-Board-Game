#include <winsock2.h>
#include "packet.h"
#include "linklist.h"
#include <pthread.h>
#include "player.h"

#pragma comment(lib,"ws2_32.lib") 

#define MAX_CLIENT 12 //Max clients can connect to server

// local variable
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;
llist *client_list;
int client_in_server = 0;

typedef struct Argument{
	Player *current_player; 
	int type;
	char message[MAX_OF_CHAR_MESSAGE];
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

void getClientInfo(Packet *pack){
	Player *client = calloc(1, sizeof(Player));
	client = createPlayer(client_addr, pack->message, ++client_in_server);
	push_llist(client_list, client);
}

void *client_handle(void *argument){
	Packet *packet = (Packet *)malloc(sizeof(Packet));
	char *message = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	Argument *arg = (Argument *)argument;
	struct node *current = *client_list;
	int length = sizeof(arg->current_player->player_client);
	enum pack_type type = (enum pack_type)arg->type;
	switch (type){
	case PUBLIC_MESSAGE_PACK:
		packet = MessageParse(arg->message);
		printf("Received packet from %s port %d: %s\n", inet_ntoa(arg->current_player->player_client.sin_addr), ntohs(arg->current_player->player_client.sin_port), arg->message);
		message = PacketParse_PubMess(packet, arg->current_player->name);
		while(current != NULL){
			Player *current_player = (Player *)current->data;
			sendto(sockfd, message, strlen(message), 0, (SOCKADDR *) &current_player->player_client, length);
			current = current->next;
		}
		break;
	case EXIT_PACK:
		printf("Exit packet received!\n");
		break;
	default:
		printf("Can't recognize packet %s!\n", arg->message);
		break;
	}
}

int main(){
	client_list = create_llist(NULL);
    int length, buffer_size;
	char *buffer = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
	char *welcome = "Welcome to our server!";
	length = sizeof(server_addr) ;
	Packet *pack = (Packet *)malloc(sizeof(Packet));

	pthread_t tid[MAX_CLIENT];

	if(setupServer() == SUCCEED_RETURN){
        printf("Server is now online...\n");
        while(1){
			memset(&client_addr, 0, sizeof(client_addr));
            memset(buffer, '\0', MAX_OF_CHAR_MESSAGE);
			ZeroMemory(pack, sizeof(pack));
            buffer_size = recvfrom(sockfd, buffer, MAX_OF_CHAR_MESSAGE, 0, (SOCKADDR *)&client_addr, &length);
			buffer[buffer_size] = '\0';
			int type =  GetPackType(buffer);
			if(type == (int)LOGIN_PACK){
				pack = MessageParse(buffer);
				getClientInfo(pack);
				sendto(sockfd, welcome, strlen(welcome), 0, (SOCKADDR *) &client_addr, length);
				Player *current_player = (Player *)get_head_data(client_list);
				printf("%s from port %d and ip %s enter chat room\n", current_player->name, ntohs(current_player->player_client.sin_port), inet_ntoa(current_player->player_client.sin_addr));
			}else{
				Player *current_player = createPlayer(client_addr, NULL, -1);
				current_player = (Player *) find_node(client_list, current_player, findPlayer);
				Argument *arg = (Argument *)malloc(sizeof(Argument));
				arg->current_player = (Player *)malloc(sizeof(Player));
				memset(arg->message, '\0', MAX_OF_CHAR_MESSAGE);
				arg->type = type;
				strcpy(arg->message, buffer);
				arg->current_player->id = current_player->id;
				strcpy(arg->current_player->name, current_player->name);
				arg->current_player->player_client = current_player->player_client;
				if(current_player != NULL){
					pthread_create(&tid[current_player->id], NULL, &client_handle, (void *)arg);
					pthread_join(tid[current_player->id], NULL);
				}
			}
        }
    }
	closeServer();
	return 0;
}