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
    int tWidth;
    int tHeight;
}GameTexture;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
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
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(renderer == NULL){
            printf("Cant create render! SDL_Error: %s\n", SDL_GetError());
            return ERROR;
        }
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) &imgFlags)){
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return ERROR;
        }
        if(TTF_Init() == -1){
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            return ERROR;
        }
    }
    return SUCCEED;
}

void load_Texture_Text(const char *text, GameTexture *current, TTF_Font *font, SDL_Color textColor){
    SDL_Surface *screen = TTF_RenderUTF8_Blended(font, text, textColor);  
    if(screen == NULL){
        printf("Cant create surface! SDL_Error: %s\n", SDL_GetError());
        exit(ERROR);
    }
    current->texture = SDL_CreateTextureFromSurface(renderer, screen);
    if(current->texture == NULL){
        printf("Cant render from surface! SDL_Error: %s\n", SDL_GetError());
        exit(ERROR);
    }
    current->tWidth = screen->w;
    current->tHeight = screen->h;
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
    current->texture = SDL_CreateTextureFromSurface(renderer, screen);
    SDL_FreeSurface(screen);
}

void ResetRender(){ //reset window to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDL_Rect Render(GameTexture *current, int width, int height){ // Render texture and display at position [x,y]
    SDL_Rect rect = {width, height, current->tWidth, current->tHeight}; 
    SDL_RenderCopy(renderer, current->texture, NULL, &rect);
    SDL_RenderPresent(renderer); //display
    SDL_DestroyTexture(current->texture);
    return rect;
}

void close_win(){ //Close the game
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void free_texture(GameTexture *current){ //free current state
    SDL_DestroyTexture(current->texture);
}

void Main_Taskbar(){ // 
    GameTexture **MenuText = (GameTexture **)malloc(sizeof(GameTexture *) * 3);
    int i;
    for(i = 0; i < 3; i++){
        MenuText[i] = (GameTexture *)malloc(sizeof(GameTexture) * 3);
    }
    TTF_Font *font = TTF_OpenFont("lib/font/text.ttf", 35);
    SDL_Color textColor = {255, 0, 0};

    char *menu[3] = {"Join Game", "Host Game", "Exit"};
    for(i = 0; i < 3; i++){
        load_Texture_Text(menu[i], MenuText[i], font, textColor);
    }
    Render(MenuText[0], (SCREEN_WIDTH - MenuText[0]->tWidth) / 2,(SCREEN_HEIGHT - MenuText[0]->tHeight * 2) / 2);
    Render(MenuText[1], (SCREEN_WIDTH - MenuText[1]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[1]->tHeight * 2) / 2);
    Render(MenuText[2], (SCREEN_WIDTH - MenuText[2]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[2]->tHeight * 6) / 2);
}

void Main_Background(){
    GameTexture *background = (GameTexture *)malloc(sizeof(GameTexture));
    load_Texture_IMG("image/bg1.bmp", background);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    background->tWidth = surface->w; //get sizof background
    background->tHeight = surface->h;
    SDL_FreeSurface(surface);
    Render(background, 0, 0);
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
    int quit = 0;
    if(CreateWindow() == 0){
        exit(ERROR);
    }
    ResetRender();
    Main_Background();
    Main_Taskbar();

    while(!quit){
        quit = Event();
        SDL_Delay(10); 
    }
    close_win();
    return SUCCEED;
}