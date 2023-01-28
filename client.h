#include <winsock2.h>
#include <unistd.h>
#include "packet.h"
#include <pthread.h>

int setupClient();
void closeClient();
void Listen_To_Server();
void Login_To_Server(char *player_name);
void client_process();
