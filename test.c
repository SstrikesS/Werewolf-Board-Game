#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOST_GAME 1
#define JOIN_GAME 2
#define EXIT_GAME 3
#define ESC -1

typedef struct GameTexture{
    SDL_Texture *texture;
    int tWidth;
    int tHeight;
}GameTexture;

int check_mouse_pos(SDL_Rect rect){
    int x, y;
    SDL_GetMouseState(&x, &y);
    int value = 1;
    if(x < rect.x){
        value = -1;
    }
    else if(x > rect.x + rect.w){
        value = -1;
    }
    else if(y < rect.y){
        value = -1;
    }
    else if(y > rect.y + rect.h){
        value = -1;
    }
    return value;
}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 770;

#define ERROR 0
#define SUCCEED 1
#define INSIDE 1
#define OUTSIDE 0

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
int InGame_Screen(int selection);
int Main_Screen(){ // 
    ResetRender();
    int i;
    int Menu_item = 3;
    SDL_Rect button[Menu_item];

    GameTexture **MenuText = (GameTexture **)malloc(sizeof(GameTexture *) * Menu_item);
    for(i = 0; i < Menu_item; i++){
        MenuText[i] = (GameTexture *)malloc(sizeof(GameTexture) * Menu_item);
    }
    GameTexture *background = (GameTexture *)malloc(sizeof(GameTexture));
    
    SDL_Color textColor = {255, 0, 0};
    SDL_Color highlight_Color = {255, 255, 0};
    TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
    char *menu[] = {"Join Game", "Host Game", "Exit"};
    int select[] = {0, 0, 0};
    SDL_Event menu_e;

    load_Texture_IMG("resource/bg1.bmp", background);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    background->tWidth = surface->w; //get sizeof background
    background->tHeight = surface->h;
    SDL_FreeSurface(surface);
    Render(background, 0, 0);

    for(i = 0; i < Menu_item; i++){
            load_Texture_Text(menu[i], MenuText[i], font, textColor);
            button[i] = Render(MenuText[i], (SCREEN_WIDTH - MenuText[i]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[0]->tHeight * (i * 4 - 2)) / 2);
            SDL_RenderPresent(renderer);
    }
    while(1){
        while (SDL_PollEvent(&menu_e)){
            switch (menu_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_MOUSEMOTION:
                for(i = 0; i < Menu_item; i++){
                    if(check_mouse_pos(button[i]) == 1){
                        if(select[i] == 0){
                            select[i] = i + 1;
                            load_Texture_Text(menu[i], MenuText[i], font, highlight_Color);
                            Render(MenuText[i], (SCREEN_WIDTH - MenuText[i]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[0]->tHeight * (i * 4 - 2)) / 2);
                            SDL_RenderPresent(renderer);
                        }
                    }
                    else{
                        if(select[i] != 0){
                            select[i] = 0;
                            load_Texture_Text(menu[i], MenuText[i], font, textColor);
                            Render(MenuText[i], (SCREEN_WIDTH - MenuText[i]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[0]->tHeight * (i * 4 - 2)) / 2);
                            SDL_RenderPresent(renderer);
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                for(i = 0; i < Menu_item; i++){
                    if(check_mouse_pos(button[i]) == 1){
                        select[i] = i + 1;
                        return select[i];
                    }
                }
            default:
                break;
            }
        }
    }
    for(i = 0; i < Menu_item; i++){
        SDL_DestroyTexture(MenuText[i]->texture);
        MenuText[i]->tHeight = 0;
        MenuText[i]->tWidth = 0;
    }
    SDL_DestroyTexture(background->texture);
    background->tHeight = 0;
    background->tWidth = 0;
    return -1;
}

int InGame_Screen(int selection){
    ResetRender();
    SDL_Event game_e;
    if(selection == EXIT_GAME){
        close_win();
        exit(0);
    }else if(selection == HOST_GAME){
        ResetRender();
        GameTexture *test = (GameTexture *)malloc(sizeof(GameTexture));
        SDL_Color textColor = {255, 255, 0};
        TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
        char *text = "This is Join Game screen!";
        load_Texture_Text(text, test, font, textColor);
        Render(test, 55, 55);
        SDL_RenderPresent(renderer);
    }else if(selection == JOIN_GAME){
        ResetRender();
        GameTexture *test2 = (GameTexture *)malloc(sizeof(GameTexture));
        SDL_Color textColor2 = {255, 255, 0};
        TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
        char *text = "This is Host Game screen!";
        load_Texture_Text(text, test2, font, textColor2);
        Render(test2, 55, 55);
        GameTexture *test3 = (GameTexture *)malloc(sizeof(GameTexture));
        test3->tWidth = 480;
        test3->tHeight = 360;
        load_Texture_IMG("resource/chatboxPic.jpg", test3);
        Render(test3, 10, 400);
        GameTexture **test4 = (GameTexture **)malloc(sizeof(GameTexture*) * 10);
        for (int i = 0; i < 10; i++) {
            test4[i] = (GameTexture*)malloc(sizeof(GameTexture));
        }
        char *inputMessage = (char*)calloc(100, sizeof(char));
        strcpy(inputMessage, "vacygiulwcilgiugcbwiacb qb i bwieb k i ur qukb rjq   q rqbb r r biusbiuceb wiq vyu bu kbwfiubewi");
        char *cutMessage = calloc(20, sizeof(char));
        SDL_Color inputMessage_Color = {117, 174, 111};
        TTF_Font *font2 = TTF_OpenFont("resource/font.ttf", 15);
        int i = 0;
        while (strlen(inputMessage) > 20)
        {
            memset(cutMessage, '\0', sizeof(char));
            strncpy(cutMessage, inputMessage, 20);
            printf("%s\n", cutMessage);
            // memcpy(inputMessage, inputMessage + 20, strlen(inputMessage) - 20);
            // inputMessage[strlen(inputMessage) - 20] = '\0';
            for (int i = 0; i < strlen(inputMessage) - 20; i++) {
                inputMessage[i] = inputMessage[i + 20];
                printf("%c", inputMessage[i]);
            }
            inputMessage[strlen(inputMessage) - 20] = '\0';
            printf("%s\n", inputMessage);
            load_Texture_Text(cutMessage, test4[i], font2, inputMessage_Color);
            test4[i]->tWidth = 300;
            // test4->tHeight = 300;
            Render(test4[i], 105, 460 + i * 20);
            i++;
        }
        SDL_RenderPresent(renderer);        
    }
    while(1){
        while(SDL_PollEvent(&game_e)){
            switch (game_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_KEYDOWN:
                if(game_e.key.keysym.sym == SDLK_ESCAPE){
                    return ESC;
                }
                break;
            default:
                break;
            }

        }
    }
    return 0;
}

int main(int argc, char** argv) {
    int quit = 0;
    if(CreateWindow() == 0){
        exit(ERROR);
    }
    int selection = 0;
    do{
        selection = Main_Screen();
        selection = InGame_Screen(selection);
    }while(selection == ESC);
    close_win();
    return SUCCEED;
}