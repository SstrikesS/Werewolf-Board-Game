#include <winsock2.h>
#include "globaldefine.h"
#include "pack.h"
#include <pthread.h>

//#pragma comment(lib,"ws2_32.lib")

#define MAX_CLIENT 100 //Max clients can connect to server
int validRoom = 0;

typedef struct Argument{
	int index;
	char* message;
}Argument;

typedef struct Room{
    char *room_id;
    int *player_id;
    int max_client;
	int client_count;
    int isStart;
}Room;

typedef struct Client{
	struct sockaddr_in client;
	char *name;
	int id;
	Room *room;
	int host;
}Client;

// local variable
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;
Client *client_list;
Room *room_list;

int setupServer(){ 
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("[-]Failed. Error Code : %d\n", WSAGetLastError());
		return ERROR_RETURN;
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		printf("[-]Could not create socket : %d\n" , WSAGetLastError());
        return ERROR_RETURN;
	}

    ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("26.57.39.175");
	server_addr.sin_port = htons(PORT);
	
	if(bind(sockfd ,(SOCKADDR *)&server_addr , sizeof(server_addr)) == SOCKET_ERROR){
		printf("[-]Bind failed with error code : %d\n" , WSAGetLastError());
		return ERROR_RETURN;
	}
    return SUCCEED_RETURN;
}

void sendToClient(SOCKET sockfd, struct sockaddr_in client_addr, char *buffer){
    int length = (int)sizeof(client_addr);
    sendto(sockfd, buffer, strlen(buffer), 0, (SOCKADDR *)&client_addr, length);
}

struct sockaddr_in ListenToClient(SOCKET sockfd, struct sockaddr_in client_addr, char *buffer){
    int buffer_size, length = sizeof(client_addr);
    buffer_size = recvfrom(sockfd, buffer, MAX_MESSAGE, 0, (SOCKADDR *)&client_addr, &length);
    buffer[buffer_size] = '\0';
    return client_addr;
}

void closeServer(){
    closesocket(sockfd);
	WSACleanup();
}

Argument *getArument(int index, char *message){
    Argument *arg = calloc(1, sizeof(Argument));
    arg->index = index;
    arg->message = calloc(MAX_MESSAGE, sizeof(char));
    memset(arg->message, 0, sizeof(*arg->message));
    strcpy(arg->message, message);
    return arg;
}

int addClient(char *client_name){
	int i;
	for(i = 0; i < MAX_CLIENT; i++){
		if(client_list[i].id == -1){
			client_list[i].id = i + 1;
			strcpy(client_list[i].name, client_name);
			client_list[i].client = client_addr;
			printf("[+]%s with id %d from ip %s is joined server!\n", client_list[i].name, client_list[i].id, inet_ntoa(client_list[i].client.sin_addr));
			return i;
		}
	}
	return ERROR_RETURN;
}

void addPlayer(Room *room, int index){
    int i;
    for(i = 0; i < room->max_client; i++){
        if(room->player_id[i] == 0){
            room->player_id[i] = client_list[index].id;
            room->client_count++;
            client_list[index].room = room;
            printf("[+]'%s' with id '%d' is joined room '%s'!\n", client_list[index].name, client_list[index].id, room->room_id);
            break;
        }
    }
}
void checkEmptyRoom(Room *room){
    if(room->client_count == 0){
        printf("[+]Room '%s' has been removed!\n", room->room_id);
        memset(room->player_id, 0 , sizeof(*room->player_id));
        room->max_client = -1;
        memset(room->room_id, 0, sizeof(*room->room_id));
    }
}

void removePlayer(Room *room, int player_id){
    int i;
    for(i = 0; i < room->client_count; i++){
        if(room->player_id[i] == player_id){
            room->player_id[i] = 0;
            room->client_count--;
            checkEmptyRoom(room);
            break;
        }
    }
}

void removeClient(int index){
    printf("[+]%s with id %d has left the server!\n", client_list[index].name, client_list[index].id);
    removePlayer(client_list[index].room, client_list[index].id);
    client_list[index].id = -1;
    memset(client_list[index].name, 0, sizeof(*client_list[index].name));
    memset(&client_list[index].client, 0, sizeof(client_list[index].client));
    client_list[index].host = -1;
}

int checkRoom(char *roomName){
    int i;
    for(i = 0; i < MAX_ROOM; i++){
        if(strcmp(room_list[i].room_id, roomName) == 0){
            return i;
        }
    }
    return 0;
}

int *countValidRoom(int *validRoom){
    int i;
    int *list = calloc(MAX_ROOM, sizeof(int));
    for(i = 0; i < MAX_ROOM; i++){
        list[i] = -1;
    }
    *validRoom = 0;
    for(i = 0; i < MAX_ROOM; i++){
        if(strlen(room_list[i].room_id) > 0 && room_list[i].client_count < room_list[i].max_client && room_list[i].isStart == -1){
            list[i] = i;
            *validRoom = *validRoom + 1;
        }
    }
    return list;
}

void GetRoom(int index){
    int i, j;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
        memset(token[i], 0, sizeof(*token[i]));
    }
    sprintf(token[0], "%d",client_list[index].room->client_count);
    sprintf(token[1], "%d",client_list[index].room->max_client);
    buffer = GetMess(token, 2, ROOM_INFO);
    sendToClient(sockfd, client_list[index].client, buffer);
    for(i = 0; i < 2; i++){
        memset(token[i], 0, sizeof(*token[i]));
    }
    j = 0;
    for(i = 0; i < client_list[index].room->max_client; i++){
        if(client_list[index].room->player_id[i] != 0){
            sprintf(token[j++], "%d", client_list[index].room->player_id[i]);
            strcpy(token[j++], client_list[client_list[index].room->player_id[i] - 1].name);
        }
    }
    memset(buffer, 0, sizeof(*buffer));
    buffer = GetMess(token, j, ROOM_INFO);
    //printf("buffer = %s\n", buffer);
    sendToClient(sockfd, client_list[index].client, buffer);
    free(buffer);
    free(token);
}

void *clientHandle(void *argument){
    int i, j = -1;
    int *list = calloc(MAX_ROOM, sizeof(int));
    pthread_detach(pthread_self());
    char *tmp, *tmp2;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
        memset(token[i], 0, sizeof(*token[i]));
    }

    Argument *arg = (Argument *) argument;
    enum pack_type type = GetType(arg->message);
    switch (type){
    case PUBLIC_MESSAGE_PACK:
        token = GetToken(arg->message, 2);
        tmp = calloc(MAX_MESSAGE, sizeof(char));
        tmp2 = calloc(MAX_MESSAGE, sizeof(char));
        
        strcat(tmp2, "You: ");
        strcat(tmp2, token[1]);

        strcat(tmp, client_list[arg->index].name);
        strcat(tmp, ": ");
        strcat(tmp, token[1]);

        for(i = 0; i < 2; i++){
            memset(token[i], 0, sizeof(*token[i]));
        }
        strcpy(token[0], tmp2);
        buffer = GetMess(token, 1, PUBLIC_MESSAGE_PACK);
        sendToClient(sockfd, client_list[arg->index].client, buffer);

        memset(buffer, 0 ,sizeof(*buffer));
        memset(token[0], 0, sizeof(*token[0]));

        strcpy(token[0], tmp);
        buffer = GetMess(token, 1, PUBLIC_MESSAGE_PACK);
        for(i = 0; i < client_list[arg->index].room->max_client; i++){
            if(client_list[arg->index].room->player_id[i] > 0){
                if(client_list[arg->index].room->player_id[i] == client_list[arg->index].id){
                    continue;
                }
                sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
            }
        }
        break;
    case EXIT_PACK:
        removeClient(arg->index);
        break;
    case HOST_GAME:
        token = GetToken(arg->message, 3);
        if(checkRoom(token[1]) != 0){
            for(i = 0; i < 3; i++){
                memset(token[i], 0, sizeof(*token[i]));
            }
            strcpy(token[0], "Room name already exists!");
            buffer = GetMess(token, 1, ERROR_PACK);
        }else{
            for(i = 0; i < MAX_ROOM; i++){
                if(strlen(room_list[i].room_id) == 0){
                    strcpy(room_list[i].room_id, token[1]);
                    room_list[i].max_client = atoi(token[2]);
                    room_list[i].player_id = calloc(room_list[i].max_client, sizeof(int));
                    room_list[i].player_id[room_list[i].client_count] = client_list[arg->index].id;
                    room_list[i].client_count++;
                    client_list[arg->index].room = &room_list[i]; 
                    break;
                }
            }
            for(i = 0; i < 3; i++){
                memset(token[i], 0, sizeof(*token[i]));
            }
            printf("[+]%s is hosting room '%s'!\n", client_list[arg->index].name, client_list[arg->index].room->room_id);
            strcpy(token[0], "OK!");
            buffer = GetMess(token, 1, SUCCEED_PACK);
            sendToClient(sockfd, client_list[arg->index].client, buffer);
        }
        GetRoom(arg->index);
        break;
    case JOIN_GAME:
        for(i = 0; i < MAX_ROOM; i++){
            list[i] = -1;
        }
        list = countValidRoom(&validRoom);
        sprintf(token[0], "%d", validRoom);
        buffer = GetMess(token, 1, JOIN_GAME);
        sendToClient(sockfd, client_list[arg->index].client, buffer);

        for(i = 0; i < 2; i++){
            memset(token[i], 0, sizeof(*token[i]));
        }
        j = 0;
        for(i = 0; i < MAX_ROOM; i++){
            if(list[i] > -1){
                strcpy(token[j++], room_list[list[i]].room_id);
                sprintf(token[j++], "%d", room_list[list[i]].max_client);
                sprintf(token[j++], "%d", room_list[list[i]].client_count);
            }
        }
        buffer = GetMess(token, j, HOST_GAME);
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        break;
    case JOIN_ROOM:
        token = GetToken(arg->message, 3);
        j = checkRoom(token[2]);
        if(j != -1){
            if(room_list[j].client_count < room_list[j].max_client){
                addPlayer(&room_list[j], arg->index);
                for(i = 0; i < 3; i++){
                    memset(token[i], 0, sizeof(*token[i]));
                }
                strcpy(token[0], "OK!");
                buffer = GetMess(token, 2, SUCCEED_PACK);
            }
        }else{
            for(i = 0; i < 3; i++){
                memset(token[i], 0, sizeof(*token[i]));
            }
            strcpy(token[0], "Room is already full!");
            buffer = GetMess(token, 2, ERROR_PACK);
        }
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        //printf("arg = %d\n", arg->index);
        for(i = 0; i < client_list[arg->index].room->max_client; i++){
            if(client_list[arg->index].room->player_id[i] != 0){
                //printf("player id = %d sent\n", client_list[arg->index].room->player_id[i]);
                GetRoom(client_list[arg->index].room->player_id[i] - 1);
            }
        }
        break;
    // case ROOM_INFO:
    //     GetRoom(arg->index);
    //     break;
    case START_GAME:
        if(client_list[arg->index].room->client_count == client_list[arg->index].room->max_client){
            strcpy(token[0], "OK!");
            buffer = GetMess(token, 1, SUCCEED_RETURN);
        }else{
            strcpy(token[0], "Not enough players to start!");
            buffer = GetMess(token, 1, ERROR_RETURN);
        }
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        //printf("%s\n", buffer);
        break;
    default:
        printf("[-]Can't recognize packet %s!\n", arg->message);
        break;
    }
    free(buffer);
    free(token);
    free(list);
    return NULL;
}

void startServer(){
    int check = -1, i;
    enum pack_type type;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
        memset(token[i], 0, sizeof(*token[i]));
    }
    pthread_t tid;
    while(1){
        memset(buffer, 0, sizeof(*buffer));
        ZeroMemory(&client_addr, sizeof(client_addr));

        client_addr = ListenToClient(sockfd, client_addr, buffer);
        // printf("[+]Buffer = %s\n", buffer);
        type = GetType(buffer);
        if(type == LOGIN_PACK){
            token = GetToken(buffer, 2);
            check = addClient(token[1]);
            for(i = 0; i < 2; i++){
                memset(token[i], 0, sizeof(*token[i]));
            }
            memset(buffer, 0, sizeof(*buffer));
            sprintf(token[0], "%d", client_list[check].id);
            if(check != ERROR_RETURN){
                strcpy(token[1],"Welcome to our server!");
                buffer = GetMess(token, 2, SUCCEED_PACK);
            }else{
                strcpy(token[1], "Login failed!");
                buffer = GetMess(token, 2, ERROR_PACK);
            }
            sendToClient(sockfd, client_addr, buffer);
        }else{
            for(i = 0; i < MAX_CLIENT; i++){
                if((client_list[i].client.sin_addr.s_addr == client_addr.sin_addr.s_addr) && (client_list[i].client.sin_port == client_addr.sin_port)){
                    Argument *arg = getArument(i, buffer);
                    pthread_create(&tid, NULL, &clientHandle, (void *)arg);
                }
            }
        }
    }
}
int main(){
    int i;
    room_list = calloc(MAX_ROOM, sizeof(Room));
	client_list = calloc(MAX_CLIENT, sizeof(Client));
	for(i = 0; i < MAX_ROOM; i++){
		room_list[i].room_id = calloc(MAX_NAME, sizeof(char));
		room_list[i].client_count = 0;
        room_list[i].isStart = -1;
	}
	for(i = 0; i < MAX_CLIENT; i++){
		client_list[i].host = -1;
		client_list[i].id = -1;
		client_list[i].name = calloc(MAX_NAME, sizeof(char));
        ZeroMemory(&client_list[i].client, sizeof(client_list[i].client));
		client_list[i].room = calloc(1, sizeof(Room)); 
        
	}
    if(setupServer() == SUCCEED_RETURN){
        printf("[+]Server is now online!\n");
        startServer();
    }
}