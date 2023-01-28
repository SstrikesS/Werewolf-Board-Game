#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "MenuGame.h"
#include "globaldefine.h"

SDL_Window *gWindow = NULL; // window of game
SDL_Renderer *gRender = NULL; // render of window

void CloseGame(){ // close the game(free resource)
    SDL_DestroyWindow(gWindow); // free window
    SDL_DestroyRenderer(gRender); // free render
    free(gWindow);
    free(gRender);
    //TTF_Quit();
    IMG_Quit(); // close SDL_image
    SDL_Quit(); // close SDL
    exit(0);
}
int StartGame(){ // start SDL and create window, render
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL could not initialize! SDL_Error %s\n", SDL_GetError());
        return ERROR_RETURN;
    }else{
        // start window 
        gWindow = SDL_CreateWindow("WereWolf Deluxo Edition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Window could not initialize! SDL_Error: %s\n", SDL_GetError());
            return ERROR_RETURN;
        }
        // start render
        gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(gRender == NULL){
            printf("Cant create render! SDL_Error: %s\n", SDL_GetError());
            return ERROR_RETURN;
        }
        // start SDL_image
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) &imgFlags)){
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return ERROR_RETURN;
        }
        // start SDL_ttf 
        if(TTF_Init() == -1){
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            return ERROR_RETURN;
        }
    }
    return SUCCEED_RETURN;
}

int main(int argc, char** argv){
    if(!StartGame()){
        CloseGame();
    }
    // start menu game
    MainMenu(gWindow, gRender);
    return 0;
}