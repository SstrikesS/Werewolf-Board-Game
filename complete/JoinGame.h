#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "globaldefine.h"
#include "WaitingRoom.h"

void JoinGame(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window, CurrentPlayer *currUser);
