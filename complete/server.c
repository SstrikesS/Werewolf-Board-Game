#include <winsock2.h>
#include "globaldefine.h"
#include "pack.h"
#include <windows.h>
#include <pthread.h>
#define CLIENT_TIMEOUT 4
#define MAX_CLIENT 100 //Max clients can connect to server
int validRoom = 0;
int count = 0;
typedef struct Argument{
	int index;
	char* message;
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
    int w;
    int v;
    int currentRole;
    int current_wolf;
}Room;

typedef struct Client{
	struct sockaddr_in client;
    int role;
	char *name;
	int id;
	Room *room;
	int host;
    int timeout;
    int heal;
    int vote_count;
    int target;
}Client;

// local variable
pthread_t tid, tid_2, tid_3;
SOCKET sockfd;
SOCKET pingfd;
struct sockaddr_in server_addr, client_addr, ping_addr;
Client *client_list;
Room *room_list;

char **token;

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
        printf("[+]Room '%s' has '%d' player(s)!\n", room->room_id, room->client_count);
        printf("[+]Room '%s' has been removed!\n", room->room_id);
        memset(room->player_id, 0 , sizeof(*room->player_id));
        room->max_client = -1;
        room->isNight = -1;
        room->isStart = -1;
        room->currentRole = -1;
        //memset(room->playerRole, 0 ,sizeof(*room->playerRole));
        memset(room->room_id, 0, sizeof(*room->room_id));
        return 0;
    }
    printf("[+]Room '%s' has '%d' player(s)!\n", room->room_id, room->client_count);
    return 1;
}

void GetRoom(int index){
    int i, j;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    sprintf(token[0], "%d",client_list[index].room->client_count);
    sprintf(token[1], "%d",client_list[index].room->max_client);
    buffer = GetMess(token, 2, ROOM_INFO);
    sendToClient(sockfd, client_list[index].client, buffer);
    cleanToken(token , 2);
    j = 0;
    for(i = 0; i < client_list[index].room->max_client; i++){
        if(client_list[index].room->player_id[i] != 0){
            sprintf(token[j++], "%d", client_list[index].room->player_id[i]);
            strcpy(token[j++], client_list[client_list[index].room->player_id[i] - 1].name);
        }
        
    }
    memset(buffer, 0, sizeof(*buffer));
    buffer = GetMess(token, j, ROOM_INFO);
    cleanToken(token ,j);
    sendToClient(sockfd, client_list[index].client, buffer);
    free(buffer);
}

void removePlayer(Room *room, int player_id){
    int i;
    for(i = 0; i < room->max_client; i++){
        if(room->player_id[i] == player_id){
            room->player_id[i] = 0;
            room->client_count--;
            printf("[+]'%s' with id '%d' has left room '%s'!\n", client_list[player_id - 1].name, player_id, client_list[player_id - 1].room->room_id);
            break;
        }
    }
}

void removeClient(int index){
    int i;
    if(client_list[index].id != -1){
        if(client_list[index].room != NULL){
            removePlayer(client_list[index].room, client_list[index].id);
            if(checkEmptyRoom(client_list[index].room)){
                if(client_list[index].host == 1){
                    printf("[+]Changing host of room '%s'!\n", client_list[index].room->room_id);
                    char *buffer = calloc(4, sizeof(char));
                    buffer = GetMess(token, 0, HOST_GAME);
                    for(i = 0; i < client_list[index].room->max_client; i++){
                        if(client_list[index].room->player_id[i] > 0 && client_list[index].id != client_list[index].room->player_id[i]){
                            sendToClient(sockfd, client_list[client_list[index].room->player_id[i] - 1].client, buffer);
                            client_list[client_list[index].room->player_id[i] - 1].host = 1;
                            printf("[+]Host of room '%s' is set to player '%s' with id '%d'!\n", client_list[index].room->room_id, client_list[client_list[index].room->player_id[i] -1].name, client_list[index].room->player_id[i]);
                            break;
                        }
                    }
                    free(buffer);
                }
                for(i = 0; i < client_list[index].room->max_client; i++){
                    if(client_list[index].room->player_id[i] != 0){
                        GetRoom(client_list[index].room->player_id[i] - 1);
                        printf("[+]Sent to other player(s)!\n");
                    }
                }
            }
        }
        printf("[+]'%s' with id '%d' has left the server!\n", client_list[index].name, client_list[index].id);
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
    removeClient(arg->index);
    return NULL;
}

int isDead(int index){// kiem tra player chet chua
    if(client_list[index].heal <= 0){
        return DEAD;
    }else{
        return ALIVE;
    }
}

void CheckGameStatus(Room *room){// check game sau 1 ngay/ 1 dem
    printf("Checking!!!!\n");
    char *buffer = calloc(10, sizeof(char));
    int i, j;
    for(i = 0; i < room->max_client; i++){
        if(room->player_id[i] > 0){
            printf("heal = %d\n",client_list[room->player_id[i] - 1].heal);
            if(isDead(room->player_id[i] - 1) == DEAD){
                printf("[+]Player '%s' with id '%d' is eliminated in room '%s'!\n", client_list[room->player_id[i] - 1].name, client_list[room->player_id[i] - 1].id, room->room_id);
                if(client_list[room->player_id[i] - 1].role == WEREWOLF){
                    room->w--; //so luong soi giam
                }else{
                    room->v--; //so luong nguoi giam
                }
                client_list[room->player_id[i] - 1].role = SPECTATOR;// chuyen role
                sprintf(token[0], "%d", room->player_id[i]);
                buffer = GetMess(token, 1, PLAYER_DEAD);
                cleanToken(token, 1);
                for(j = 0; j < room->max_client; j++){
                    sendToClient(sockfd, client_list[room->player_id[j] - 1].client, buffer);
                }
            }
        }
    }
    //Note: Sau moi dem phai reset mau(player.heal) ve 1 neu player do con song(ALIVE)
    for(i = 0; i < room->max_client; i++){
        if(client_list[room->player_id[i] - 1].role != SPECTATOR){
            client_list[room->player_id[i] - 1].heal = 1;
            client_list[room->player_id[i] - 1].vote_count = 0;
        }
    }
    for(i = 0; i < room->max_client; i++){
        printf("%d -> %d\n", room->player_id[i], client_list[room->player_id[i] - 1].heal);
    }
    if(room->v <= room->w){
        printf("[+]Werewolfs win!!!!!!!!!!!\n");
        return;
    }else if(room->w == 0){
        printf("[+]Villages win!!!!!!!!!!!!\n");
        return;
    }
    if(room->isNight != 1){
        printf("In\n");
        strcpy(token[0], "NextDay");
        buffer = GetMess(token, 1, PLAYER_TURN);
        cleanToken(token, 1);
        for(i = 0; i < room->max_client; i++){
            sendToClient(sockfd, client_list[room->player_id[i] - 1].client, buffer);
        }
    }else if(room->isNight == 1){
        int j = 0;
        printf("In2");
        while(j == 0){
            for(i = 0; i < room->max_client; i++){
                if(client_list[room->player_id[i] - 1].role == room->currentRole){
                    j++;
                    strcpy(token[0] , "Night");
                    buffer = GetMess(token, 1, PLAYER_TURN);
                    cleanToken(token, 1);
                    sendToClient(sockfd, client_list[room->player_id[i] - 1].client, buffer);
                }
            }
            if(j == 0){
                room->currentRole++;
                printf("[+]Current role in room '%s' is '%d'!\n", room->room_id, room->currentRole);
            }
        }
    }
    printf("End check!!!!\n");
}

void target(Argument *arg, Client * player1 , Client * player2, int skill){
    char *buffer = calloc(4, sizeof(char));
    switch (player1->role){
        case WEREWOLF:
            printf("[+]Player '%d' votes kill player '%d'!\n", player1->id, player2->id);
            player2->vote_count++; // bi can mat 1 mau
            break;
        case WITCH:
            if(skill == WITCH_PROTECT){
                printf("[+]Player '%d' throws protect potion to player '%d'!\n", player1->id, player2->id);
                player2->heal++;
            }else if(skill == WITCH_POISION){
                printf("[+]Player '%d' throws poison to player '%d'!\n", player1->id, player2->id);
                player2->heal -= 2;
            }
            break;
        case PROTECTER:
            printf("[+]Player '%d' protects player '%d'!\n", player1->id, player2->id);
            player2->heal = player2->heal + 1;
            //printf("%d\n" , player2->heal);
            //printf("this is protector\n");
            break;
        case HUNTER:
            printf("[+]Player '%d' hunts player '%d'!\n", player1->id, player2->id);
            player1->target = player2->id;
            break;
        case SEER:
            printf("[+]Player '%d' saw role of player '%d'!\n", player1->id, player2->id);
            if(player2->role == WEREWOLF){
                sprintf(token[0], "%d", WEREWOLF);
                buffer = GetMess(token, 1, PLAYER_TARGET);
                sendToClient(sockfd, client_list[arg->index].client, buffer);
            }else{
                sprintf(token[0], "%d", VILLAGE);
                buffer = GetMess(token, 1, PLAYER_TARGET);
                sendToClient(sockfd, client_list[arg->index].client, buffer);
            }
            cleanToken(token , 1);
            break;
        default:
            break;
    }
    free(buffer);
}

void Night_handle(Argument *arg){
    char *buffer = calloc(100, sizeof(char*));
    int i, j;
    if(client_list[arg->index].room->currentRole == PROTECTER){
        client_list[arg->index].room->currentRole = WEREWOLF;
        if(atoi(token[1]) >= 0){
            target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
        }
    }
    else if(client_list[arg->index].room->currentRole == WEREWOLF){
        client_list[arg->index].room->current_wolf++;
        if(client_list[arg->index].room->current_wolf < client_list[arg->index].room->w){
            if(atoi(token[1]) >= 0){
                target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
            }
            return;
        }
        if(client_list[arg->index].room->current_wolf == client_list[arg->index].room->w){
            if(atoi(token[1]) >= 0){
                target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
            }   
            client_list[arg->index].room->currentRole = SEER;
            client_list[arg->index].room->current_wolf = 0;
        }
    }else if(client_list[arg->index].room->currentRole == SEER){
        if(client_list[arg->index].room->max_client >= 11){
            if(atoi(token[1]) >= 0){
                target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
            }
            client_list[arg->index].room->currentRole = WITCH;
        }else{
            if(atoi(token[1]) >= 0){
                target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
            }
            client_list[arg->index].room->currentRole = HUNTER;
        }
    }else if(client_list[arg->index].room->currentRole == WITCH){
        if(atoi(token[1]) >= 0){
            target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
        }
        client_list[arg->index].room->currentRole = HUNTER;
    }else if(client_list[arg->index].room->currentRole == HUNTER){
        if(atoi(token[1]) >= 0){
            target(arg , &client_list[arg->index], &client_list[atoi(token[1]) - 1], atoi(token[2]));
            printf("Hunter's target is %d!\n", client_list[arg->index].target);
        }
        client_list[arg->index].room->currentRole = GOOD_MORNING;
    }
    cleanToken(token, 3);
    printf("[+]Current role in room '%s' is '%d'!\n", client_list[arg->index].room->room_id, client_list[arg->index].room->currentRole);
    j = 0;
    while(j == 0){
        for(i = 0; i < client_list[arg->index].room->max_client; i++){
            if(client_list[client_list[arg->index].room->player_id[i] - 1].role == client_list[arg->index].room->currentRole){
                j++;
                strcpy(token[0] , "Night");
                buffer = GetMess(token, 1, PLAYER_TURN);
                cleanToken(token, 1);
                sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
            }
        }
        if(client_list[arg->index].room->currentRole == GOOD_MORNING){
            client_list[arg->index].room->isNight = -1;
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                if(client_list[client_list[arg->index].room->player_id[i] - 1].vote_count > (int)(client_list[arg->index].room->current_wolf / 2) + 1){
                    client_list[client_list[arg->index].room->player_id[i] - 1].heal--;
                }
            }
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                if(client_list[client_list[arg->index].room->player_id[i] - 1].role == HUNTER && client_list[client_list[arg->index].room->player_id[i] - 1].target != -1 && client_list[client_list[arg->index].room->player_id[i] - 1].heal <= 0){
                    printf("i'm here!\n");
                    client_list[client_list[client_list[arg->index].room->player_id[i] - 1].target - 1].heal -= 2;
                    client_list[client_list[arg->index].room->player_id[i] - 1].target = -1;
                }
            }
            printf("[+]Room '%s' GoodMoring!\n", client_list[arg->index].room->room_id);
            CheckGameStatus(client_list[arg->index].room);
            break;
        }
        if(j == 0){
            client_list[arg->index].room->currentRole++;
            printf("[+]Current role in room '%s' is '%d'!\n", client_list[arg->index].room->room_id, client_list[arg->index].room->currentRole);
        }
    }
    free(buffer);
}

void Day_handle(Argument *arg){
    int i;
    count++;
    if(atoi(token[1]) >= 0){
        client_list[atoi(token[1]) - 1].vote_count++;
    }
    if(count == client_list[arg->index].room->v + client_list[arg->index].room->w){
        for(i = 0; i < client_list[arg->index].room->max_client; i++){
            if(client_list[client_list[arg->index].room->player_id[i] - 1].vote_count > (int)((client_list[arg->index].room->w + client_list[arg->index].room->v) / 2) + 1){
                client_list[client_list[arg->index].room->player_id[i] - 1].heal--;
            }
        }
        printf("[+]Room '%s' GoodNight!\n", client_list[arg->index].room->room_id);
        client_list[arg->index].room->isNight = 1;
        client_list[arg->index].room->currentRole = PROTECTER;
        CheckGameStatus(client_list[arg->index].room);
    }
}
void *clientHandle(void *argument){
    pthread_detach(pthread_self());
    int i, j = -1;
    int *list = calloc(MAX_ROOM, sizeof(int));
    char *tmp, *tmp2;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    Argument *arg = (Argument *) argument;
    enum pack_type type = GetType(arg->message);
    printf("[+]Client with id '%d': %s\n", client_list[arg->index].id, arg->message);
    switch (type){
    case LOGIN_PACK:
        token = GetToken(arg->message, 2);
        addClient(arg->index, token[1], arg->client);
        cleanToken(token, 2);
        sprintf(token[0], "%d", client_list[arg->index].id);
        if(arg->index != ERROR_RETURN){
            buffer = GetMess(token, 1, SUCCEED_PACK);
        }else{
            buffer = GetMess(token, 1, ERROR_PACK);
        }
        cleanToken(token, 1);
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        pthread_create(&tid_3, NULL, timeoutClient, (void*)arg);
        break;
    case PUBLIC_MESSAGE_PACK:
        token = GetToken(arg->message, 2);
        tmp = calloc(MAX_MESSAGE, sizeof(char));
        tmp2 = calloc(MAX_MESSAGE, sizeof(char));
        
        strcat(tmp2, "You: ");
        strcat(tmp2, token[1]);

        strcat(tmp, client_list[arg->index].name);
        strcat(tmp, ": ");
        strcat(tmp, token[1]);
        cleanToken(token, 2);
        strcpy(token[0], tmp2);
        buffer = GetMess(token, 1, PUBLIC_MESSAGE_PACK);
        sendToClient(sockfd, client_list[arg->index].client, buffer);

        memset(buffer, 0 ,sizeof(*buffer));
        cleanToken(token, 1);

        strcpy(token[0], tmp);
        buffer = GetMess(token, 1, PUBLIC_MESSAGE_PACK);
        cleanToken(token, 1);
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
        client_list[arg->index].timeout = 0;
        break;
    case HOST_GAME:
        token = GetToken(arg->message, 3);
        if(checkRoom(token[1]) != 0){
            cleanToken(token, 3);
            strcpy(token[0], "Room name already exists!");
            buffer = GetMess(token, 1, ERROR_PACK);
            cleanToken(token, 1);
        }else{
            for(i = 0; i < MAX_ROOM; i++){
                if(strlen(room_list[i].room_id) == 0){
                    strcpy(room_list[i].room_id, token[1]);
                    room_list[i].max_client = atoi(token[2]);
                    room_list[i].player_id = calloc(room_list[i].max_client, sizeof(int));
                    room_list[i].player_id[room_list[i].client_count] = client_list[arg->index].id;
                    room_list[i].client_count++;
                    client_list[arg->index].room = &room_list[i];
                    client_list[arg->index].host = 1;
                    break;
                }
            }
            cleanToken(token, 3);

            printf("[+]%s is hosting room '%s' max player is '%d'!\n", client_list[arg->index].name, client_list[arg->index].room->room_id, client_list[arg->index].room->max_client);
            strcpy(token[0], "OK!");
            buffer = GetMess(token, 1, SUCCEED_PACK);
            cleanToken(token, 1);
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
        cleanToken(token, 1);

        j = 0;
        for(i = 0; i < MAX_ROOM; i++){
            if(list[i] > -1){
                strcpy(token[j++], room_list[list[i]].room_id);
                sprintf(token[j++], "%d", room_list[list[i]].max_client);
                sprintf(token[j++], "%d", room_list[list[i]].client_count);
            }
        }
        buffer = GetMess(token, j, HOST_GAME);
        cleanToken(token, j);
        sendToClient(sockfd, client_list[arg->index].client, buffer);
        break;
    case JOIN_ROOM:
        token = GetToken(arg->message, 3);
        j = checkRoom(token[2]);
        if(j != -1){
            if(room_list[j].client_count < room_list[j].max_client && room_list[j].isStart != 1){
                addPlayer(&room_list[j], arg->index);
                cleanToken(token, 3);
                strcpy(token[0], "OK!");
                buffer = GetMess(token, 2, SUCCEED_PACK);
                cleanToken(token, 2);
                sendToClient(sockfd, client_list[arg->index].client, buffer);
                for(i = 0; i < client_list[arg->index].room->max_client; i++){
                    if(client_list[arg->index].room->player_id[i] != 0){
                        GetRoom(client_list[arg->index].room->player_id[i] - 1);
                    }
                }
            }
        }else{
            cleanToken(token, 3);
            strcpy(token[0], "Room is already full!");
            buffer = GetMess(token, 2, ERROR_PACK);
            cleanToken(token, 2);
            sendToClient(sockfd, client_list[arg->index].client, buffer);
        }
        break;
    case START_GAME:
        if(client_list[arg->index].room->client_count == client_list[arg->index].room->max_client){
            if(client_list[arg->index].room->max_client == 8){
                // int arr[] = {PROTECTER, WEREWOLF, WEREWOLF};
                // int arr[] = {PROTECTER, PROTECTER, PROTECTER, PROTECTER, PROTECTER, PROTECTER, PROTECTER, PROTECTER};
                client_list[arg->index].room->w = 2;
                client_list[arg->index].room->v = 6;
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 9){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->w = 2;
                client_list[arg->index].room->v = 7;
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 10){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->w = 3;
                client_list[arg->index].room->v = 7;
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 11){
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, WITCH, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->w = 3;
                client_list[arg->index].room->v = 8;
                client_list[arg->index].room->playerRole = arr;
            }else if(client_list[arg->index].room->max_client == 12){
                client_list[arg->index].room->w = 4;
                client_list[arg->index].room->v = 8;
                int arr[] = {VILLAGE, VILLAGE, VILLAGE, VILLAGE, WEREWOLF, WEREWOLF, WEREWOLF, WEREWOLF, WITCH, PROTECTER, SEER, HUNTER};
                client_list[arg->index].room->playerRole = arr;
            }
            shuffle(client_list[arg->index].room->playerRole, client_list[arg->index].room->client_count);
            j = -1;
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                
                
                client_list[client_list[arg->index].room->player_id[i] - 1].role = client_list[arg->index].room->playerRole[i];
                if(client_list[client_list[arg->index].room->player_id[i] - 1].role == WEREWOLF){
                    if(j == -1){
                        j = client_list[arg->index].room->player_id[i];
                    }else{
                        sprintf(token[0], "%d", j);
                        buffer = GetMess(token, 1, ROLE);
                        cleanToken(token, 1);
                        sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
                        sprintf(token[0], "%d", client_list[arg->index].room->player_id[i]);
                        buffer = GetMess(token, 1, ROLE);
                        cleanToken(token, 1);
                        sendToClient(sockfd, client_list[j - 1].client, buffer);
                    }
                }
                memset(buffer, 0, sizeof(*buffer));
                sprintf(token[0], "%d", (int)client_list[arg->index].room->playerRole[i]);
                buffer = GetMess(token, 1, START_GAME);
                cleanToken(token, 1);
                sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
            }
            j = -1;
            for(i = 0; i < client_list[arg->index].room->max_client; i++){
                if(client_list[client_list[arg->index].room->player_id[i] - 1].role == PROTECTER){
                    client_list[arg->index].room->currentRole = PROTECTER;
                    strcpy(token[0], "Night");
                    buffer = GetMess(token, 1, PLAYER_TURN);
                    cleanToken(token, 1);
                    sendToClient(sockfd, client_list[client_list[arg->index].room->player_id[i] - 1].client, buffer);
                    break;   
                }
            }
            
            client_list[arg->index].room->isStart = 1;
            client_list[arg->index].room->isNight = 1;
        }
        break;
    case PLAYER_TARGET:
        token = GetToken(arg->message, 3);
        if(client_list[arg->index].room->isNight == 1){
            Night_handle(arg);
        }else{
            Day_handle(arg);
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
    token = makeCleanToken();
	for(i = 0; i < MAX_ROOM; i++){
		room_list[i].room_id = calloc(MAX_NAME, sizeof(char));
		room_list[i].client_count = 0;
        room_list[i].isStart = -1;
        room_list[i].isNight = -1;
        room_list[i].currentRole = -1;
        room_list[i].current_wolf = 0;
	}
	for(i = 0; i < MAX_CLIENT; i++){
        client_list[i].role = -1;
		client_list[i].host = -1;
		client_list[i].id = -1;
		client_list[i].name = calloc(MAX_NAME, sizeof(char));
        client_list[i].heal = 1;
        client_list[i].vote_count = 0;
        client_list[i].target = -1;
        ZeroMemory(&client_list[i].client, sizeof(client_list[i].client));
		client_list[i].room = NULL; 
	}
    pthread_create(&tid_2, NULL, listenPingServer, NULL);
    if(setupServer() == SUCCEED_RETURN){
        printf("[+]Server is now online!\n");
        startServer();
    }
}