#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct GameStatus{
    SDL_Texture *texture;
    SDL_Renderer *renderer;
}GameStatus;
SDL_Window *window = NULL;
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 770;
#define ERROR 0
#define SUCCEED 1

int CreateWindow(){ // Create window
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL could not initialize! SDL_Error %s\n", SDL_GetError());
        return ERROR;
    }else{
        window = SDL_CreateWindow("WereWolf Deluxo Edition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf("Window could not initialize! SDL_Error: %s\n", SDL_GetError());
            return ERROR;
        }
    }
    return SUCCEED;
}

void load_Texture(char *path, GameStatus *current){ // Load texture
    SDL_Surface *screen = NULL;
    screen = SDL_LoadBMP(path);
    if(screen == NULL){
        printf("Cant create surface! SDL_Error: %s\n", SDL_GetError());
        exit(ERROR);
    }
    current->texture = SDL_CreateTextureFromSurface(current->renderer, screen);
    SDL_FreeSurface(screen);
}

void Render(GameStatus *current){
    SDL_SetRenderDrawColor(current->renderer, 255, 0, 0, 255);
    SDL_RenderClear(current->renderer);
    SDL_SetRenderDrawColor(current->renderer, 255, 255, 255, 255);
    //SDL_Rect rect = {64, 64, 64, 64};
    SDL_RenderCopy(current->renderer, current->texture, NULL, NULL);
    SDL_RenderPresent(current->renderer);
}

void close_win(){ //Close the game
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

void free_texture(GameStatus *current){
    SDL_DestroyTexture(current->texture);
    SDL_DestroyRenderer(current->renderer);
}

int Event(GameStatus *current){
    SDL_Event e;
    int value = 0;
    while(SDL_PollEvent(&e)){
        switch (e.type){
            case SDL_QUIT:
                value = 1;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                if(window){
                    close_win();
                    value = 1;
                }
                break;
            default:
                break;  
        }
    }
    return value;
}
int main(int argc, char** argv) {
    GameStatus *current = (GameStatus *)malloc(sizeof(GameStatus));
    if(CreateWindow() == 0){
        printf("Failed to initialize!\n");
        close_win();
    }
    current->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    load_Texture("image/bg1.bmp", current);
    int quit = 0; 
    while(!quit){
        quit = Event(current);
        Render(current);
        SDL_Delay(100); 
    }
    free_texture(current);
    return SUCCEED;
}