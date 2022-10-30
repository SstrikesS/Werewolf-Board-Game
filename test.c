#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 770;
int init(SDL_Window *window){ // Initialize SDL
    int success = 1;
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL could not initialize! SDL_Error %s\n", SDL_GetError());
        success = 0;
    }else{
        window = SDL_CreateWindow("WereWolf Deluxo Edition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf("Window could not initialize! SDL_Error: %s\n",SDL_GetError());
            success = 0;
        }else{
            //load_main_menu_background(window);
            load_main_menu_option(window);
        }
    }
    return success;
}
int load_main_menu_background(SDL_Window *window){ // Load main menu
    int success = 1;
    SDL_Surface *screen;
    SDL_Surface *background;
    screen = SDL_GetWindowSurface(window);
    background = SDL_LoadBMP("image/bg1.bmp");
    if(background == NULL){
        printf("SDL_Error: %s\n", SDL_GetError());
        success = 0;
    }
    SDL_BlitSurface(background , NULL, screen, NULL);
    SDL_FreeSurface(background);
    SDL_UpdateWindowSurface(window);
    return success;
}
void load_main_menu_option(SDL_Window *window){ //Load option
    TTF_Font * font = TTF_OpenFont("Sans.ttf", 24);
    SDL_Renderer *renderer;
    SDL_Color White = {255,0,0};
    SDL_Surface * textSurface = TTF_RenderText_Solid(font, "asdijfdasfjkasf", White);
    SDL_Texture * textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect
    SDL_RenderCopy(renderer, textTexture, NULL, &Message_rect);
    // SDL_DestroyTexture(textTexture);
    // SDL_DestroyRenderer(renderer);
}
void close_win(SDL_Window *window){ //Close the game
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}
int main(int argc, char** argv) {
    SDL_Window *window = NULL;
    if(init(window) == 0){
        printf("Failed to initialize!\n");
    }else{
        SDL_Event e; 
        int quit = 0; 
        while( quit == 0 ){ 
            while( SDL_PollEvent(&e)){ 
                if(e.type == SDL_QUIT) 
                    quit = 1; 
            } 
        }
    }
    return 0;
}