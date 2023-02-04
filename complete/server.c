#include <winsock2.h>
#include "globaldefine.h"
#include "pack.h"
#include <windows.h>
#include <pthread.h>
#define CLIENT_TIMEOUT 4
#define MAX_CLIENT 100 //Max clients can connect to server
int validRoom = 0;

typedef struct Argument{
	int index;
	char* message;
    char **token;
    struct sockaddr_in client;
}Argument;

typedef struct Room{
    char *room_id;
    int *player_id;
    int max_client;
    int *playerRole;
	int client_count;
    int isStart;
    int isNight;
}Room;

typedef struct Client{
	struct sockaddr_in client;
    int role;
	char *name;
	int id;
	Room *room;
	int host;
    int timeout;
}Client;

// local variable
pthread_t tid, tid_2, tid_3;
SOCKET sockfd;
SOCKET pingfd;
struct sockaddr_in server_addr, client_addr, ping_addr;
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
    if((pingfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
        printf("[-]Could not create socket : %d\n" , WSAGetLastError());
        return ERROR_RETURN;
    }

    ZeroMemory(&server_addr, sizeof(server_addr));
    ZeroMemory(&ping_addr, sizeof(ping_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	server_addr.sin_port = htons(PORT);

    ping_addr.sin_family = AF_INET;
	ping_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	ping_addr.sin_port = htons(PORT + 1);

    if(bind(pingfd ,(SOCKADDR *)&ping_addr , sizeof(ping_addr)) == SOCKET_ERROR){
		printf("[-]Bind failed with error code : %d\n" , WSAGetLastError());
		return ERROR_RETURN;
	}
	
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

Argument *getArument(int index, char *message, struct sockaddr_in client){
    Argument *arg = calloc(1, sizeof(Argument));
    arg->index = index;
    arg->message = calloc(MAX_MESSAGE, sizeof(char));
    arg->client = client;
    memset(arg->message, 0, sizeof(*arg->message));
    strcpy(arg->message, message);
    return arg;
}

void addClient(int index, char *client_name, struct sockaddr_in client){
    client_list[index].id = index + 1;
    strcpy(client_list[index].name, client_name);
    client_list[index].client = client;
    printf("[+]%s with id %d from ip %s is joined server!\n", client_list[index].name, client_list[index].id, inet_ntoa(client_list[index].client.sin_addr));
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

int checkEmptyRoom(Room *room){
    if(room->client_count == 0){
        printf("[+]Room '%s' has been removed!\n", room->room_id);
        memset(room->player_id, 0 , sizeof(*room->player_id));
        room->max_client = -1;
        memset(room->room_id, 0, sizeof(*room->room_id));
        return 0;
    }
    return 1;
}

void GetRoom(int index, char **token){
    int i, j;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
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
    for(i = 0; i < j; i++){
        memset(token[i], 0, sizeof(*token[i]));
    }
    sendToClient(sockfd, client_list[index].client, buffer);
    free(buffer);
}

void removePlayer(Room *room, int player_id){
    int i;
    for(i = 0; i < room->max_client; i++){
        if(room->player_id[i] == player_id){
            room->player_id[i] = 0;
            room->client_count--;
            break;
        }
    }
    
}

void removeClient(int index, char **token){
    int i;
    if(client_list[index].id != -1){
        printf("[+]%s with id %d has left the server!\n", client_list[index].name, client_list[index].id);
        removePlayer(client_list[index].room, client_list[index].id);
        if(checkEmptyRoom(client_list[index].room)){
            if(client_list[index].host == 1){
                char *buffer = calloc(12, sizeof(char));
                buffer = GetMess(token, 0, HOST_GAME);
                for(i = 0; i < client_list[index].room->max_client; i++){
                    if(client_list[index].room->player_id[i] > 0 && client_list[index].id != client_list[index].room->player_id[i]){
                        sendToClient(sockfd, client_list[client_list[index].room->player_id[i] - 1].client, buffer);
                        client_list[client_list[index].room->player_id[i] - 1].host = 1;
                        break;
                    }
                }
                free(buffer);
            }
            for(i = 0; i < client_list[index].room->max_client; i++){
                if(client_list[index].room->player_id[i] != 0){
                    GetRoom(client_list[index].room->player_id[i] - 1, token);
                }
            }
        }
        client_list[index].id = -1;
        memset(client_list[index].name, 0, sizeof(*client_list[index].name));
        ZeroMemory(&client_list[index].client, sizeof(client_list[index].client));
        client_list[index].host = -1;
    }
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

void shuffle(int array[], int length){
    srand( time(NULL) );
    for (int i = 0; i < length; i++){
        int swap_index = rand() % length;
        int temp = array[i];
        array[i] = array[swap_index];
        array[swap_index] = temp;
    }
}

void *timeoutClient(void *argument){
    pthread_detach(pthread_self());
    Argument *arg = (Argument *)argument;
    client_list[arg->index].timeout = CLIENT_TIMEOUT;
    do{
        Sleep(1000);
        client_list[arg->index].timeout--;
    }while (client_list[arg->index].timeout > 0);
    removeClient(arg->index, arg->token);
    return NULL;
}

void *clientHandle(void *argument){
    pthread_detach(pthread_self());
    int i, j = -1;
    int *list = calloc(MAX_ROOM, sizeof(int));
    char *tmp, *tmp2;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    Argument *arg = (Argument *) argument;
    enum pack_type type = GetType(arg->message);
    free(arg->token);
    switch (type){
    case LOGIN_PACK:
        arg->token = GetToken(arg->message, 2);
        addClient(arg->index, arg->token[1], arg->client);
        for(i = 0; i < 2; i++){
            memset(arg->token[i], 0, sizeof(*arg->token[i]));
        }
        sprintf(arg->token[0], "%d", client_list[arg->index].id);
        if(arg->index != ERROR_RETURN){
            buffer = GetMess(arg->token, 1, SUCCEED_PACK);
        }else{
            buffer = GetMess(arg->token, 1, ERROR_PACK);
        }
        memset(arg->token[0], 0, sizeof(*arg->token[0]));
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        pthread_create(&tid_3, NULL, timeoutClient, (void*)arg);
        break;
    case PUBLIC_MESSAGE_PACK:
        arg->token = GetToken(arg->message, 2);
        tmp = calloc(MAX_MESSAGE, sizeof(char));
        tmp2 = calloc(MAX_MESSAGE, sizeof(char));
        
        strcat(tmp2, "You: ");
        strcat(tmp2, arg->token[1]);

        strcat(tmp, client_list[arg->index].name);
        strcat(tmp, ": ");
        strcat(tmp, arg->token[1]);

        for(i = 0; i < 2; i++){
            memset(arg->token[i], 0, sizeof(*arg->token[i]));
        }
        strcpy(arg->token[0], tmp2);
        buffer = GetMess(arg->token, 1, PUBLIC_MESSAGE_PACK);
        sendToClient(sockfd, client_list[arg->index].client, buffer);

        memset(buffer, 0 ,sizeof(*buffer));
        memset(arg->token[0], 0, sizeof(*arg->token[0]));

        strcpy(arg->token[0], tmp);
        buffer = GetMess(arg->token, 1, PUBLIC_MESSAGE_PACK);
        memset(arg->token[0], 0, sizeof(*arg->token[0]));
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
        removeClient(arg->index, arg->token);
        break;
    case HOST_GAME:
        arg->token = GetToken(arg->message, 3);
        if(checkRoom(arg->token[1]) != 0){
            for(i = 0; i < 3; i++){
                memset(arg->token[i], 0, sizeof(*arg->token[i]));
            }
            strcpy(arg->token[0], "Room name already exists!");
            buffer = GetMess(arg->token, 1, ERROR_PACK);
            memset(arg->token[0], 0, sizeof(*arg->token[0]));
        }else{
            for(i = 0; i < MAX_ROOM; i++){
                if(strlen(room_list[i].room_id) == 0){
                    strcpy(room_list[i].room_id, arg->token[1]);
                    room_list[i].max_client = atoi(arg->token[2]);
                    room_list[i].player_id = calloc(room_list[i].max_client, sizeof(int));
                    room_list[i].player_id[room_list[i].client_count] = client_list[arg->index].id;
                    room_list[i].client_count++;
                    client_list[arg->index].room = &room_list[i];
                    client_list[arg->index].host = 1;
                    break;
                }
            }
            for(i = 0; i < 3; i++){
                memset(arg->token[i], 0, sizeof(*arg->token[i]));
            }

            printf("[+]%s is hosting room '%s' max player is '%d'!\n", client_list[arg->index].name, client_list[arg->index].room->room_id, client_list[arg->index].room->max_client);
            strcpy(arg->token[0], "OK!");
            buffer = GetMess(arg->token, 1, SUCCEED_PACK);
            memset(arg->token[0], 0, sizeof(*arg->token[0]));
            sendToClient(sockfd, client_list[arg->index].client, buffer);
        }
        GetRoom(arg->index, arg->token);
        break;
    case JOIN_GAME:
        arg->token = makeCleanToken();
        for(i = 0; i < MAX_ROOM; i++){
            list[i] = -1;
        }
        list = countValidRoom(&validRoom);
        sprintf(arg->token[0], "%d", validRoom);
        buffer = GetMess(arg->token, 1, JOIN_GAME);
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        memset(arg->token[0], 0, sizeof(*arg->token[0]));

        j = 0;
        for(i = 0; i < MAX_ROOM; i++){
            if(list[i] > -1){
                strcpy(arg->token[j++], room_list[list[i]].room_id);
                sprintf(arg->token[j++], "%d", room_list[list[i]].max_client);
                sprintf(arg->token[j++], "%d", room_list[list[i]].client_count);
            }
        }
        buffer = GetMess(arg->token, j, HOST_GAME);
        for(j = 0; i < j; i++){
            memset(arg->token[i], 0, sizeof(*arg->token[i]));
        }
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        break;
    case JOIN_ROOM:
        free(arg->token);
        arg->token = GetToken(arg->message, 3);
        j = checkRoom(arg->token[2]);
        if(j != -1){
            if(room_list[j].client_count < room_list[j].max_client && room_list[j].isStart != 1){
                addPlayer(&room_list[j], arg->index);
                for(i = 0; i < 3; i++){
                    memset(arg->token[i], 0, sizeof(*arg->token[i]));
                }
                strcpy(arg->token[0], "OK!");
                buffer = GetMess(arg->token, 2, SUCCEED_PACK);
                for(i = 0; i < 2; i++){
                    memset(arg->token[i], 0, sizeof(*arg->token[i]));
                }
                sendToClient(sockfd, client_list[arg->index].client, buffer);
                for(i = 0; i < client_list[arg->index].room->max_client; i++){
                    if(client_list[arg->index].room->player_id[i] != 0){
                        GetRoom(client_list[arg->index].room->player_id[i] - 1, arg->token);
                    }
                }
            }
        }else{
            for(i = 0; i < 3; i++){
                memset(arg->token[i], 0, sizeof(*arg->token[i]));
            }
            strcpy(arg->token[0], "Room is already full!");
            buffer = GetMess(arg->token, 2, ERROR_PACK);
            for(i = 0; i < 2; i++){
                memset(arg->token[i], 0, sizeof(*arg->token[i]));
            }
            sendToClient(sockfd, client_list[arg->index].client, buffer);
        }
        break;
    case START_GAME:
        if(client_list[arg->index].room->client_count == client_list[arg->index].room->max_client){
            if(client_list[arg->index].room->max_client == 8){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 9){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 10){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 11){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, WITCH, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 12){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, WEREWOLF, WITCH, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }
            shuffle(client_list[arg->index].room->playerRole, client_list[arg->index].room->max_client);
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                client_list[client_list[arg->index].room->player_id[i] - 1].role = client_list[arg->index].room->playerRole[i];
                // printf("%d->%d\n", client_list[arg->index].room->player_id[i], client_list[arg->index].room->playerRole[i]);
            }
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                memset(arg->token[0], 0 ,sizeof(*arg->token[0]));
                memset(buffer, 0, sizeof(*buffer));
                sprintf(arg->token[0], "%d", (int)client_list[arg->index].room->playerRole[i]);
                buffer = GetMess(arg->token, 1, START_GAME);
                memset(arg->token[0], 0, sizeof(*arg->token[0]));
                sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
            }
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                if(client_list[client_list[arg->index].room->player_id[i] - 1].role == PROTECTER){
                    buffer = GetMess(arg->token, 0, PLAYER_TURN);
                    sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
                    break;
                }
            }
            client_list[arg->index].room->isStart = 1;
            client_list[arg->index].room->isNight = 1;
        }
        break;
    default:
        printf("[-]Can't recognize packet %s!\n", arg->message);
        break;
    }
    free(buffer);
    free(list);
    return NULL;
}

void *listenPingServer(){
    pthread_detach(pthread_self());
    int i;
    char *buffer = calloc(10, sizeof(char));
    struct sockaddr_in client;
    while(1){
        memset(buffer, 0, sizeof(*buffer));
        ZeroMemory(&client, sizeof(client));
        client = ListenToClient(pingfd, client_addr, buffer);
        if(strlen(buffer) > 0){
            for(i = 0; i < MAX_CLIENT; i++){
                if(atoi(buffer) == client_list[i].id){
                    client_list[i].timeout = CLIENT_TIMEOUT;
                    break;
                }
            }
        }
    }
    return NULL;
}

void startServer(){
    int i;
    enum pack_type type;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    while(1){
        memset(buffer, 0, sizeof(*buffer));
        ZeroMemory(&client_addr, sizeof(client_addr));
        client_addr = ListenToClient(sockfd, client_addr, buffer);
        type = GetType(buffer);
        if(type == LOGIN_PACK){
            for(i = 0; i < MAX_CLIENT; i++){
                if(client_list[i].id == -1){
                    Argument *arg = getArument(i, buffer, client_addr);
                    pthread_create(&tid, NULL, &clientHandle, (void *)arg);
                    break;
                }
            }
        }else{
            for(i = 0; i < MAX_CLIENT; i++){
                if((client_list[i].client.sin_addr.s_addr == client_addr.sin_addr.s_addr) && (client_list[i].client.sin_port == client_addr.sin_port)){
                    Argument *arg = getArument(i, buffer, client_addr);
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
        room_list[i].isNight = -1;
	}
	for(i = 0; i < MAX_CLIENT; i++){
        client_list[i].role = -1;
		client_list[i].host = -1;
		client_list[i].id = -1;
		client_list[i].name = calloc(MAX_NAME, sizeof(char));
        ZeroMemory(&client_list[i].client, sizeof(client_list[i].client));
		client_list[i].room = calloc(1, sizeof(Room)); 
	}
    pthread_create(&tid_2, NULL, listenPingServer, NULL);
    if(setupServer() == SUCCEED_RETURN){
        printf("[+]Server is now online!\n");
        startServer();
    }
}