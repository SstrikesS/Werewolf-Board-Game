#include <winsock2.h>
#include "definelink.h"

typedef struct Client{
	struct sockaddr_in client;
	char *name;
	int id;
	char *room_id;
	int host;
}Client;

typedef struct Room{
    char *room_id;
    int max_client;
	int client_curr;
}Room;

int findPlayer(void *tmp1, void *tmp2);

Client *createPlayer(struct sockaddr_in client, char *name, int id);

void PlayerCopy(Client *tmp1, Client *tmp2);