#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "globaldefine.h"
#include "JoinGame.h"

// SDL_Color white_color = {255, 255, 255};
// SDL_Color black_color = {0, 0, 0};
// SDL_Color red_color = {255, 0, 0};
// SDL_Color yellow_color = {255, 255, 0};

void HostGame(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window, CurrentPlayer *currUser);

