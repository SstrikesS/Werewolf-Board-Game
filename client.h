#include <winsock2.h>
#include <unistd.h>
#include "packet.h"
#include <pthread.h>

int setupClient();
void closeClient();
void Listen_To_Server(char * buffer);
char *Login_To_Server();
void client_process();
