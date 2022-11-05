#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct GameTexture{
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    int tWidth;
    int tHeight;
}GameTexture;
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

void load_Texture_Text(char *text, GameTexture *current, TTF_Font *font, SDL_Color textColor){

    SDL_Surface *screen = NULL; 
    screen = TTF_RenderUTF8_Blended(font, text, textColor);
    if(screen == NULL){
        printf("Cant create surface! SDL_Error: %s\n", SDL_GetError());
    }
    current->texture = SDL_CreateTextureFromSurface(current->renderer, screen);
    SDL_QueryTexture(current->texture, NULL, NULL, &current->tWidth, &current->tHeight);
    SDL_FreeSurface(screen);
}

void load_Texture_IMG(char *path, GameTexture *current){ // Load texture
    SDL_Surface *screen = NULL;
    screen = IMG_Load(path);
    //screen = SDL_LoadBMP(path); //can use both BMP vs IMG
    if(screen == NULL){
        printf("Cant create surface! SDL_Error: %s\n", SDL_GetError());
        exit(ERROR);
    }
    current->texture = SDL_CreateTextureFromSurface(current->renderer, screen);
    SDL_FreeSurface(screen);
}

void ResetRender(GameTexture *current){ //reset window to black
    SDL_SetRenderDrawColor(current->renderer, 0, 0, 0, 255);
    SDL_RenderClear(current->renderer);
    SDL_RenderPresent(current->renderer);
}

void Render(GameTexture *current, int width, int height){ // Render texture and display at position [x,y]
    SDL_Rect rect = {width, height, current->tWidth, current->tHeight}; 
    SDL_RenderCopy(current->renderer, current->texture, NULL, &rect);
    SDL_RenderPresent(current->renderer); //display
}

void close_win(){ //Close the game
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}

void free_texture(GameTexture *current){ //free current state
    SDL_DestroyTexture(current->texture);
    SDL_DestroyRenderer(current->renderer);
}

int Event(){ // event of the game
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
    GameTexture *background = (GameTexture *)malloc(sizeof(GameTexture));
    GameTexture *textTest = (GameTexture *)malloc(sizeof(GameTexture));
    if(CreateWindow() == 0){
        printf("Failed to initialize!\n");
        close_win();
    }
    background->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    textTest->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    load_Texture_IMG("image/bg1.bmp", background);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    background->tWidth = surface->w; //get sizof background
    background->tHeight = surface->h;
    
    SDL_FreeSurface(surface);
    TTF_Font *font = TTF_OpenFont("lib/font/text.ttf", 28);
    SDL_Color textColor = {255, 0, 0};
    char *text = "Hello World";
    load_Texture_Text(text, textTest, font, textColor);
    int quit = 0; 
    ResetRender(background);
    while(!quit){
        quit = Event();
        Render(background, 43, 60);
        //Render(textTest, 64, 64);
        SDL_Delay(100); 
    }
    TTF_CloseFont(font);
    free_texture(background);
    return SUCCEED;
}