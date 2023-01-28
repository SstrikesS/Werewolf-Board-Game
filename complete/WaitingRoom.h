#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "globaldefine.h"
#include "client.h"

int check_mouse_pos(SDL_Rect rect);
void WaitingRoom(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window, CurrentPlayer *currUser);