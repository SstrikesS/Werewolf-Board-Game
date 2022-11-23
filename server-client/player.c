#include "player.h"
#include <stdio.h>

int findPlayer(void *tmp1, void *tmp2){
    Player *player1 = (Player *)tmp1;
    Player *player2 = (Player *)tmp2;
    if(player1->player_client.sin_port == player2->player_client.sin_port && player1->player_client.sin_addr.s_addr == player2->player_client.sin_addr.s_addr){
        return 1;
    }else{
        return 0;
    }
}

Player *createPlayer(struct sockaddr_in player_client, char *name, int id){
    Player *player = (Player *)malloc(sizeof(Player));
	player->player_client = player_client;
	player->id = id;
    if(name != NULL){
	    strcpy(player->name, name);
    }
    return player;
}

void PlayerCopy(Player *tmp1, Player *tmp2){
	tmp1->id = tmp2->id;
	strcpy(tmp1->name, tmp2->name);
	tmp1->player_client = tmp2->player_client;
}