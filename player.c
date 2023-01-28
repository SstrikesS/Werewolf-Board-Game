#include "player.h"
#include <stdio.h>

int findPlayer(void *tmp1, void *tmp2){
    Client *player1 = (Client *)tmp1;
    Client *player2 = (Client *)tmp2;
    if(player1->id == player2->id){
        return 1;
    }else{
        return 0;
    }
}

Client *createPlayer(struct sockaddr_in client, char *name, int id){
    Client *player = (Client *)malloc(sizeof(Client));
	player->client = client;
	player->id = id;
    player->room_id = calloc(20, sizeof(char));
    player->host = 0;
    player->name = calloc(MAX_OF_NAME, sizeof(char));
    if(name != NULL){
	    strcpy(player->name, name);
    }
    return player;
}

void PlayerCopy(Client *tmp1, Client *tmp2){
	tmp1->id = tmp2->id;
	strcpy(tmp1->name, tmp2->name);
	tmp1->client = tmp2->client;
    strcpy(tmp1->room_id, tmp2->room_id);
    tmp1->host = tmp2->host;
}