#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERROR_RETURN -1
#define SUCCEED_RETURN 1

#define SCREEN_WIDTH 1000 //window width
#define SCREEN_HEIGHT 600 //screen height

#define MAX_NAME 20 // max character of player name
#define MAX_RNAME 30
#define MAX_MESSAGE 1000 // max character of message
#define MAX_ROOM 20
#define MAX_CLIENT 100

#define PORT 27015 //defaut port to server listen
#define PPORT 27016 //default port to get ping
#define SERVER_IP_ADDR "26.141.211.77" //the ip address of server

#define WEREWOLF 2
#define WITCH 4
#define PROTECTER 1
#define SEER 3
#define HUNTER 5
#define VILLAGE 6
#define SPECTATOR 7
#define GOOD_MORNING 0

#define WITCH_PROTECT 1
#define WITCH_POISION -1
#define DEAD -1
#define ALIVE 1
