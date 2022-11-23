#include <winsock2.h>
#include "definelink.h"

typedef struct Player{
	struct sockaddr_in player_client;
	char name[MAX_OF_NAME];
	int id;
}Player;

int findPlayer(void *tmp1, void *tmp2);

Player *createPlayer(struct sockaddr_in player_client, char *name, int id);

void PlayerCopy(Player *tmp1, Player *tmp2);