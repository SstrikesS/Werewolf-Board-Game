#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    SDL_Window *window = NULL;
    SDL_Surface *screen;
    SDL_Surface *image;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Error\n");
        return 1;
    }
    window = SDL_CreateWindow("WereWolf Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1040, 770, 0);
    if(window == NULL){
        printf("Error2\n");
        return 1;
    }
    screen = SDL_GetWindowSurface(window);
    image = SDL_LoadBMP("image/bg1.bmp");
    SDL_BlitSurface(image , NULL, screen, NULL);
    SDL_FreeSurface(image);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(3000);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}