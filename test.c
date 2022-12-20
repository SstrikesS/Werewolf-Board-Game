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
#define MAX_CHAR 55
#define ERROR 0
#define SUCCEED 1
char *text;
char *composition;
Sint32 cursor;
Sint32 selection_len;

typedef struct GameTexture{
    SDL_Texture *texture;
    int tWidth;
    int tHeight;
}GameTexture;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 770;

char **MessageLine;
int current_line = 0;
SDL_Rect inputMessage_rect;
SDL_Rect chatBox_rect;
SDL_Rect send_Button;
SDL_Rect showMessage_rect;
GameTexture *sendButton;
SDL_Rect *messLine_rect;
SDL_Color white_color = {255, 255, 255};
SDL_Color black_color = {0, 0, 0};
SDL_Color red_color = {255, 0, 0};
SDL_Color yellow_color = {255, 255, 0};

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
    // SDL_Surface *screen = TTF_RenderUTF8_Blended(font, text, textColor);
    SDL_Surface *screen = TTF_RenderText_Solid(font, text, textColor);
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

void ClearRenderRect(SDL_Rect rect){
    SDL_Rect tmp = rect;
    tmp.h = tmp.h - 2;
    tmp.w = tmp.w - 2;
    tmp.x = tmp.x + 1;
    tmp.y = tmp.y + 1;
    SDL_RenderFillRect(renderer, &tmp);
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
            load_Texture_Text(menu[i], MenuText[i], font, red_color);
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
                            load_Texture_Text(menu[i], MenuText[i], font, yellow_color);
                            Render(MenuText[i], (SCREEN_WIDTH - MenuText[i]->tWidth) / 2,(SCREEN_HEIGHT + MenuText[0]->tHeight * (i * 4 - 2)) / 2);
                            SDL_RenderPresent(renderer);
                        }
                    }
                    else{
                        if(select[i] != 0){
                            select[i] = 0;
                            load_Texture_Text(menu[i], MenuText[i], font, red_color);
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

void ChatBoxUI(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //chatbox
    chatBox_rect.h = 360;
    chatBox_rect.w = 360;
    chatBox_rect.x = 10;
    chatBox_rect.y = 400;
    SDL_RenderDrawRect(renderer, &chatBox_rect);
    //inputMessageBox
    inputMessage_rect.h = 22;
    inputMessage_rect.w = 290;
    inputMessage_rect.x = 15;
    inputMessage_rect.y = 730;
    SDL_RenderDrawRect(renderer, &inputMessage_rect);
    //showMessageBox
    showMessage_rect.h = 310;
    showMessage_rect.w = 350;
    showMessage_rect.x = 15;
    showMessage_rect.y = 410;
    SDL_RenderDrawRect(renderer, &showMessage_rect);
    //send Button
    send_Button.h = 22;
    send_Button.w = 55;
    send_Button.x = 310;
    send_Button.y = 730;
    SDL_RenderDrawRect(renderer, &send_Button);
    char *send = " Send";

    sendButton = calloc(1, sizeof(GameTexture));
    TTF_Font *font = TTF_OpenFont("resource/times.ttf", 15);
    load_Texture_Text(send, sendButton, font, white_color);
    
    Render(sendButton, 315, 731);
    SDL_RenderPresent(renderer);
}
void splitText(char *inputText) {
    char *inputMessage = (char*)calloc(200, sizeof(char));
    memset(inputMessage, '\0', 200);
    strcat(inputMessage, "You: ");
    strcat(inputMessage, inputText);
    inputMessage[strlen(inputMessage)] = '\0';
    int i = 0;
    while (strlen(inputMessage) > MAX_CHAR)
    {
        memset(MessageLine[current_line], '\0', sizeof(char));
        strncpy(MessageLine[current_line], inputMessage, MAX_CHAR);
        MessageLine[current_line][strlen(MessageLine[current_line])] = '\0';
        printf("%s\n", MessageLine[current_line]);
        for (int i = 0; i < strlen(inputMessage) - MAX_CHAR; i++) {
            inputMessage[i] = inputMessage[i + MAX_CHAR];
        }
        inputMessage[strlen(inputMessage) - MAX_CHAR] = '\0';
        i++;
        current_line++;
    }
    if (strlen(inputMessage) <= MAX_CHAR) {
        strcpy(MessageLine[current_line], inputMessage);
        current_line++;
    }     
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
        TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
        char *text = "This is Join Game screen!";
        load_Texture_Text(text, test, font, yellow_color);
        Render(test, 55, 55);
        SDL_RenderPresent(renderer);
    }else if(selection == JOIN_GAME){
        ResetRender();
        GameTexture *test2 = (GameTexture *)malloc(sizeof(GameTexture));
        TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
        char *text = "This is Host Game screen!";
        load_Texture_Text(text, test2, font, yellow_color);
        Render(test2, 55, 55);
        ChatBoxUI(); 
    }
    int i;
    SDL_Rect *messLine_rect = calloc(14, sizeof(SDL_Rect));
    MessageLine = calloc(1024 , sizeof(char *));
    for(i = 0; i < 1024; i++){
        MessageLine[i] = calloc(MAX_CHAR + 1, sizeof(char));
    }
    for(i = 0; i < 14; i++){
        messLine_rect[i].h = 22;
        messLine_rect[i].w = 340;
        messLine_rect[i].x = 20;
        messLine_rect[i].y = 420 + i * 20;
    }
    char *textMessage = calloc(100, sizeof(char));
    GameTexture *inputTexture = calloc(1, sizeof(GameTexture));
    TTF_Font *font = TTF_OpenFont("resource/times.ttf", 15);
    memset(textMessage, '\0', sizeof(char));
    int tmp;
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
                else if(game_e.key.keysym.sym == SDLK_BACKSPACE){
                    textMessage[strlen(textMessage) - 1] = '\0';
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(inputMessage_rect);
                    SDL_RenderPresent(renderer);
                }else if((game_e.key.keysym.sym == SDLK_KP_ENTER || game_e.key.keysym.sym == SDLK_RETURN) && strlen(textMessage) != 0){
                    splitText(textMessage);
                    tmp = current_line;
                    printf("%s\n", textMessage);
                    memset(textMessage, '\0', sizeof(char));
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(showMessage_rect);
                    for(i = 0 ; i < 13; i++){
                        if(current_line >= i + 1){
                            GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                            load_Texture_Text(MessageLine[current_line - i - 1], showMessage, font, white_color);
                            if(showMessage->tWidth > showMessage_rect.w - 20){
                                showMessage->tWidth = showMessage_rect.w - 20;
                            }
                            Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                        }
                    }
                    ClearRenderRect(inputMessage_rect);
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_TEXTINPUT:
                if(strlen(textMessage) < 100){
                    strcat(textMessage, game_e.text.text);
                }
                break;
            case SDL_TEXTEDITING:
                composition = game_e.edit.text;
                cursor = game_e.edit.start;
                selection_len = game_e.edit.length;
                break;
            case SDL_MOUSEMOTION:
                if(check_mouse_pos(send_Button) == 1){
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    ClearRenderRect(send_Button);
                    load_Texture_Text(" Send", sendButton, font, black_color);
                    Render(sendButton, 315, 731);
                    SDL_RenderPresent(renderer);
                }else{
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(send_Button);
                    load_Texture_Text(" Send", sendButton, font, white_color);
                    Render(sendButton, 315, 731);
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(send_Button) == 1 && strlen(textMessage) != 0){
                    splitText(textMessage);
                    tmp = current_line;
                    memset(textMessage, '\0', sizeof(char));
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(showMessage_rect);
                    for(i = 0 ; i < 13; i++){
                        if(current_line >= i + 1){
                            GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                            load_Texture_Text(MessageLine[current_line - i - 1], showMessage, font, white_color);
                            if(showMessage->tWidth > showMessage_rect.w - 20){
                                showMessage->tWidth = showMessage_rect.w - 20;
                            }
                            Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                        }else{
                            break;
                        }
                    }
                    ClearRenderRect(inputMessage_rect);
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_MOUSEWHEEL:
                if(check_mouse_pos(chatBox_rect) == 1){
                    if(game_e.wheel.y > 0 && tmp >= 14){
                        tmp--;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        ClearRenderRect(showMessage_rect);
                        for(i = 0 ; i < 13; i++){
                            if(tmp >= i + 1){
                                GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                                load_Texture_Text(MessageLine[tmp - i - 1], showMessage, font, white_color);
                                Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                            }else{
                                break;
                            }
                        }
                    }else if(game_e.wheel.y < 0 && tmp < current_line){
                        tmp++;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        ClearRenderRect(showMessage_rect);
                        for(i = 0 ; i < 13; i++){
                            if(tmp >= i + 1){
                                GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                                load_Texture_Text(MessageLine[tmp - i - 1], showMessage, font, white_color);
                                Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                            }else{
                                break;
                            }
                        }
                    }
                }
                break;
            default:
                if(strlen(textMessage) != 0){
                    load_Texture_Text(textMessage, inputTexture, font, white_color);
                    if(inputTexture->tWidth > inputMessage_rect.w - 12){
                        inputTexture->tWidth = inputMessage_rect.w - 12;
                    }
                    Render(inputTexture, 20, 729);
                    SDL_RenderPresent(renderer);
                }
                break;
            }
        }
    }
    return 0;
}

int main(int argc, char** argv) {
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