#include "client.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JOIN_GAME 1
#define HOST_GAME 2
#define EXIT_GAME 3
#define ESC -1
#define MAX_CHAR 55
#define ERROR 0
#define SUCCEED 1

typedef struct GameTexture{
    SDL_Texture *texture;
    int tWidth;
    int tHeight;
}GameTexture;

struct sockaddr_in server_addr;
SOCKET sockfd;
WSADATA wsa;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 770;
char *player_name;
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

SDL_Rect CreateRect(int x, int y, int w, int h){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.h = h;
    rect.w = w;
    return rect;
}

int CreateWindowGame(){ // Create window
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

void load_Texture_Text(const char *text, GameTexture *current, TTF_Font *font, SDL_Color textColor){ // Load text texture
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

void load_Texture_IMG(char *path, GameTexture *current){ // Load image texture
    SDL_Surface *screen = NULL;
    screen = IMG_Load(path);
    if(screen == NULL){
        printf("Cant create surface! SDL_Error: %s\n", SDL_GetError());
        exit(ERROR);
    }
    current->texture = SDL_CreateTextureFromSurface(renderer, screen);
    SDL_FreeSurface(screen);
}

void destroy_Texture(GameTexture *current){ // free texture
    SDL_DestroyTexture(current->texture);
    current->tHeight = 0;
    current->tWidth = 0;
    free(current);
}

void ResetRender(){ //reset window to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void ClearRenderRect(SDL_Rect rect){ //fill color (set render color before use it) inside rect
    SDL_Rect tmp = rect;
    tmp = CreateRect(tmp.x + 1, tmp.y + 1, tmp.w - 2, tmp.h - 2);
    SDL_RenderFillRect(renderer, &tmp);
}

SDL_Rect Render(GameTexture *current, int width, int height){ // Render texture and display at position [x,y]
    SDL_Rect rect = CreateRect(width, height, current->tWidth, current->tHeight); 
    SDL_RenderCopy(renderer, current->texture, NULL, &rect);
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
void InGame_Screen();
void JoinGame();
void HostGame();
void Main_Screen(){  
    ResetRender();
    // khoi tao 
    int i;
    SDL_Rect button[3];
    SDL_Rect player_name_rect;
    GameTexture **menuTexture = (GameTexture **)malloc(sizeof(GameTexture *) * 3);
    for(i = 0; i < 3; i++){
        menuTexture[i] = (GameTexture *)malloc(sizeof(GameTexture) * 3);
    }
    GameTexture *background = (GameTexture *)malloc(sizeof(GameTexture));
    //load font menu
    TTF_Font *font = TTF_OpenFont("resource/font.ttf", 50);
    char *menu_text[] = {"Join Game", "Host Game", "Exit"};
    SDL_Event menu_e;
    //load image background
    load_Texture_IMG("resource/bg1.bmp", background);
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    background->tWidth = surface->w; //get sizeof background
    background->tHeight = surface->h;
    SDL_FreeSurface(surface);
    Render(background, 0, 0);
    //load ten player
    player_name_rect = CreateRect(400, 250, 250, 50);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &player_name_rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    ClearRenderRect(player_name_rect);
    //render ten player
    GameTexture *player_name_texture = calloc(1, sizeof(GameTexture));
    TTF_Font *font2 = TTF_OpenFont("resource/arial.ttf", 42);
    load_Texture_Text(player_name, player_name_texture, font2, white_color);
    Render(player_name_texture, player_name_rect.x + 3, player_name_rect.y - 3);
    //render menu button
    for(i = 0; i < 3; i++){
            load_Texture_Text(menu_text[i], menuTexture[i], font, red_color);
            button[i] = Render(menuTexture[i], (SCREEN_WIDTH - menuTexture[i]->tWidth) / 2,(SCREEN_HEIGHT + menuTexture[0]->tHeight * (i * 4 - 2)) / 2);
    }
    SDL_RenderPresent(renderer);

    while(1){
        while (SDL_PollEvent(&menu_e)){
            switch (menu_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_MOUSEMOTION://di chuyen chuot
                for(i = 0; i < 3; i++){
                    if(check_mouse_pos(button[i]) == 1){
                        load_Texture_Text(menu_text[i], menuTexture[i], font, yellow_color);
                        Render(menuTexture[i], (SCREEN_WIDTH - menuTexture[i]->tWidth) / 2,(SCREEN_HEIGHT + menuTexture[0]->tHeight * (i * 4 - 2)) / 2);
                        SDL_RenderPresent(renderer);
                    }
                    else{
                        load_Texture_Text(menu_text[i], menuTexture[i], font, red_color);
                        Render(menuTexture[i], (SCREEN_WIDTH - menuTexture[i]->tWidth) / 2,(SCREEN_HEIGHT + menuTexture[0]->tHeight * (i * 4 - 2)) / 2);
                        SDL_RenderPresent(renderer);
                    }
                }
                if(check_mouse_pos(player_name_rect) == 1){
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    ClearRenderRect(player_name_rect);
                    if(strlen(player_name) != 0){
                        load_Texture_Text(player_name, player_name_texture, font2, black_color);
                        Render(player_name_texture, player_name_rect.x + 3, player_name_rect.y - 3);
                    }
                    SDL_RenderPresent(renderer);
                }else{
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(player_name_rect);
                    if(strlen(player_name) != 0){
                        load_Texture_Text(player_name, player_name_texture, font2, white_color);
                        Render(player_name_texture, player_name_rect.x + 3, player_name_rect.y - 3);
                    }
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_MOUSEBUTTONDOWN://khi chuot click
                if(strlen(player_name) != 0){
                    if(check_mouse_pos(button[0]) == 1){
                        JoinGame();
                    }else if(check_mouse_pos(button[1]) == 1){
                        HostGame();
                    }
                }
                if(check_mouse_pos(button[2]) == 1){
                    close_win();
                    exit(0);
                }
                break;
            case SDL_TEXTINPUT://khi co input tu ban phim
                if(strlen(player_name) < 10){
                    strcat(player_name, menu_e.text.text);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    ClearRenderRect(player_name_rect);
                    if(strlen(player_name) != 0){
                        load_Texture_Text(player_name, player_name_texture, font2, black_color);
                        Render(player_name_texture, player_name_rect.x + 3, player_name_rect.y - 3);
                    }
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_KEYDOWN://khi co input key tren ban phim
                if(menu_e.key.keysym.sym == SDLK_BACKSPACE){
                    player_name[strlen(player_name) - 1] = '\0';
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    ClearRenderRect(player_name_rect);
                    if(strlen(player_name) != 0){
                        load_Texture_Text(player_name, player_name_texture, font2, black_color);
                        Render(player_name_texture, player_name_rect.x + 3, player_name_rect.y - 3);
                    }
                    SDL_RenderPresent(renderer);
                }
            default:
                break;
            }
        }
    }
    for(i = 0; i < 3; i++){
        destroy_Texture(menuTexture[i]);
    }
    destroy_Texture(background);
    destroy_Texture(player_name_texture);
}

void JoinGame(){
    ResetRender();
    SDL_Event joinG_e;
    TTF_Font *font = TTF_OpenFont("resource/font.ttf", 60);
    TTF_Font *font2 = TTF_OpenFont("resource/font.ttf", 40);
    SDL_Rect button[3];
    GameTexture *list_title_texture = calloc(1, sizeof(GameTexture));
    char *list_title_text = calloc(1, sizeof(20));
    strcpy(list_title_text, "Room list");
    list_title_text[strlen(list_title_text)] = '\0';
    
    SDL_Rect list_box = CreateRect(200, 120, 750, 600);

    GameTexture **button_texture = calloc(3, sizeof(GameTexture *));
    char **button_text = calloc(3, sizeof(char *));
    int i;
    for(i = 0; i < 3; i++){
        button_texture[i] = calloc(1, sizeof(GameTexture));
        button_text[i] = calloc(10, sizeof(char));
    }

    strcpy(button_text[0], "Join");
    button_text[0][strlen(button_text[0])] = '\0';
    strcpy(button_text[1], "refresh");
    button_text[1][strlen(button_text[1])] = '\0';
    strcpy(button_text[2], "Back");
    button_text[2][strlen(button_text[2])] = '\0';

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &list_box);
    load_Texture_Text(list_title_text, list_title_texture, font, red_color);
    Render(list_title_texture, 350, 30);

    for(i = 0; i < 3; i++){
        load_Texture_Text(button_text[i], button_texture[i], font2, red_color);
        button[i] = Render(button_texture[i], 40, 240 + i * 160);                 
    }

    SDL_RenderPresent(renderer);
    while(1){
        while(SDL_PollEvent(&joinG_e)){
            switch (joinG_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_KEYDOWN:
                if(joinG_e.key.keysym.sym == SDLK_ESCAPE){
                    Main_Screen();
                }
            case SDL_MOUSEMOTION:
                for(i = 0; i < 3; i++){
                    if(check_mouse_pos(button[i]) == 1){
                        load_Texture_Text(button_text[i], button_texture[i], font2, yellow_color);
                        Render(button_texture[i], 40, 240 + i * 160);
                        SDL_RenderPresent(renderer);
                    }
                    else{
                        load_Texture_Text(button_text[i], button_texture[i], font2, red_color);
                        Render(button_texture[i], 40, 240 + i * 160);
                        SDL_RenderPresent(renderer);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(button[3]) == 1){
                    Main_Screen();
                }
                break;
            default:
                break;
            }
        }
    }
}

void HostGame(){
    Login_To_Server(player_name);
    ResetRender();
    SDL_Event hostG_e;
    TTF_Font *font = TTF_OpenFont("resource/font.ttf", 40);
    TTF_Font *font2 = TTF_OpenFont("resource/arial.ttf", 35);
    TTF_Font *font3 = TTF_OpenFont("resource/arial.ttf", 60);
    SDL_Rect roomBox = CreateRect(350, 230, 400, 330);
    SDL_Rect textBox = CreateRect(380, 300, 340, 60);
    SDL_Rect button;
    SDL_Rect button_num[2];
    SDL_Rect numRect = CreateRect(530, 440, 50, 50);
    GameTexture *maxTexture = calloc(1, sizeof(GameTexture)); 
    GameTexture *numTexture = calloc(1, sizeof(GameTexture));
    GameTexture *inTexture = calloc(1, sizeof(GameTexture));
    GameTexture *deTexture = calloc(1, sizeof(GameTexture));
    GameTexture *boxTexture = calloc(1, sizeof(GameTexture));
    GameTexture *buttonTexture = calloc(1, sizeof(GameTexture));
    GameTexture *inputTexture = calloc(1, sizeof(GameTexture));
    char *boxText = calloc(20, sizeof(char));
    char *button_text = calloc(5, sizeof(char));
    char *roomName = calloc(20, sizeof(char));
    char *numText = calloc(5, sizeof(char));
    char *maxText = calloc(20, sizeof(char));
    
    strcat(roomName, player_name);
    strcat(roomName, "'s Game");
    roomName[strlen(roomName)] = '\0';
    strcpy(boxText, "Room name");
    boxText[strlen(boxText)] = '\0';
    strcpy(button_text, "OK");
    button_text[strlen(button_text)] = '\0';
    char *inText = ">";
    char *deText = "<";
    strcpy(numText, "8");
    numText[strlen(numText)] = '\0';
    strcpy(maxText, "Max players");
    maxText[strlen(maxText)] = '\0';

    load_Texture_Text(boxText, boxTexture, font, red_color);
    Render(boxTexture, 460, 250);

    load_Texture_Text(button_text, buttonTexture, font, red_color);
    button = Render(buttonTexture, 530, 510);

    load_Texture_Text(roomName, inputTexture, font2, white_color);
    Render(inputTexture, 390, 310);

    load_Texture_Text(inText, inTexture, font3, red_color);
    button_num[0] = Render(inTexture, 680, 430);

    load_Texture_Text(deText, deTexture, font3, red_color);
    button_num[1] = Render(deTexture, 390, 430);

    load_Texture_Text(numText, numTexture, font, white_color);
    Render(numTexture, 535, 450);

    load_Texture_Text(maxText, maxTexture, font, red_color);
    Render(maxTexture, 440, 390);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &roomBox);
    SDL_RenderDrawRect(renderer, &textBox);

    SDL_RenderPresent(renderer);
    while(1){
        while(SDL_PollEvent(&hostG_e)){
            switch (hostG_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_MOUSEMOTION:
                if(check_mouse_pos(button) == 1){
                    load_Texture_Text(button_text, buttonTexture, font, yellow_color);
                    Render(buttonTexture, 530, 510);
                    SDL_RenderPresent(renderer);
                }else{
                    load_Texture_Text(button_text, buttonTexture, font, red_color);
                    Render(buttonTexture, 530, 510);
                    SDL_RenderPresent(renderer);
                }
                if(check_mouse_pos(button_num[0]) == 1){
                    load_Texture_Text(inText, inTexture, font3, yellow_color);
                    Render(inTexture, 680, 430);
                    SDL_RenderPresent(renderer);
                }else{
                    load_Texture_Text(inText, inTexture, font3, red_color);
                    Render(inTexture, 680, 430);
                    SDL_RenderPresent(renderer);
                }
                if(check_mouse_pos(button_num[1]) == 1){
                    load_Texture_Text(deText, deTexture, font3, yellow_color);
                    Render(deTexture, 390, 430);
                    SDL_RenderPresent(renderer);
                }else{
                    load_Texture_Text(deText, deTexture, font3, red_color);
                    Render(deTexture, 390, 430);
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(button) == 1){
                    if(strlen(roomName) != 0){
                        InGame_Screen();
                    }
                }
                if(check_mouse_pos(button_num[0]) == 1){
                    int tmp = atoi(numText);
                    if(tmp < 12){
                    tmp++;
                        memset(numText, '\0', 5);
                        sprintf(numText, "%d", tmp);
                        SDL_SetRenderDrawColor(renderer ,0, 0, 0, 255);
                        ClearRenderRect(numRect);
                        load_Texture_Text(numText, numTexture, font, white_color);
                        Render(numTexture, 535, 450);
                        SDL_RenderPresent(renderer);
                    }
                }else if(check_mouse_pos(button_num[1]) == 1){
                    int tmp = atoi(numText);
                    if(tmp > 8){
                        tmp--;
                        memset(numText, '\0', 5);
                        sprintf(numText, "%d", tmp);
                        SDL_SetRenderDrawColor(renderer ,0, 0, 0, 255);
                        ClearRenderRect(numRect);
                        load_Texture_Text(numText, numTexture, font, white_color);
                        Render(numTexture, 535, 450);
                        SDL_RenderPresent(renderer);
                    }
                }
                break;
            case SDL_TEXTINPUT:
                if(strlen(roomName) < 17){
                    strcat(roomName, hostG_e.text.text);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(textBox);
                    if(strlen(roomName) != 0){
                        load_Texture_Text(roomName, inputTexture, font2, white_color);
                        Render(inputTexture, 390, 310);
                    }
                    SDL_RenderPresent(renderer);
                }
                break;
            case SDL_KEYDOWN:
                if(hostG_e.key.keysym.sym == SDLK_BACKSPACE){
                    roomName[strlen(roomName) - 1] = '\0';
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(textBox);
                    if(strlen(roomName) != 0){
                        load_Texture_Text(roomName, inputTexture, font2, white_color);
                        Render(inputTexture, 390, 310);
                    }
                    SDL_RenderPresent(renderer);
                }
                if(hostG_e.key.keysym.sym == SDLK_RETURN || hostG_e.key.keysym.sym == SDLK_KP_ENTER){
                    if(strlen(roomName) != 0){
                        InGame_Screen();
                    }
                }
                if(hostG_e.key.keysym.sym == SDLK_ESCAPE){
                    Main_Screen();
                }
                break;
            default:
                break;
            }
        }
    }
}
void ChatBoxUI(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //chatbox
    chatBox_rect = CreateRect(10, 400, 360, 360);
    SDL_RenderDrawRect(renderer, &chatBox_rect);
    //inputMessageBox
    inputMessage_rect = CreateRect(15, 730, 290, 22);
    SDL_RenderDrawRect(renderer, &inputMessage_rect);
    //showMessageBox
    inputMessage_rect = CreateRect(15, 410, 350, 310);
    SDL_RenderDrawRect(renderer, &showMessage_rect);
    //send Button
    send_Button = CreateRect(310, 730, 55, 22);
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

void InGame_Screen(){
    ResetRender();
    ChatBoxUI();
    SDL_Event game_e;
    int i, tmp;
    SDL_Rect *messLine_rect = calloc(14, sizeof(SDL_Rect));
    MessageLine = calloc(1024 , sizeof(char *));
    for(i = 0; i < 1024; i++){
        MessageLine[i] = calloc(MAX_CHAR + 1, sizeof(char));
    }
    for(i = 0; i < 14; i++){
        messLine_rect[i] = CreateRect(20, 420 + i * 20, 340, 22);
    }
    char *textMessage = calloc(100, sizeof(char));
    GameTexture *inputTexture = calloc(1, sizeof(GameTexture));
    TTF_Font *font = TTF_OpenFont("resource/times.ttf", 15);
    memset(textMessage, '\0', sizeof(char));
    while(1){
        while(SDL_PollEvent(&game_e)){
            switch (game_e.type){
            case SDL_QUIT:
                close_win();
                exit(0);
                break;
            case SDL_KEYDOWN:
                if(game_e.key.keysym.sym == SDLK_ESCAPE){
                    Main_Screen();
                }
                else if(game_e.key.keysym.sym == SDLK_BACKSPACE){
                    textMessage[strlen(textMessage) - 1] = '\0';
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    ClearRenderRect(inputMessage_rect);
                    if(strlen(textMessage) != 0){
                        load_Texture_Text(textMessage, inputTexture, font, white_color);
                        if(inputTexture->tWidth > inputMessage_rect.w - 12){
                            inputTexture->tWidth = inputMessage_rect.w - 12;
                        }
                        Render(inputTexture, 20, 729);
                    }
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
                if(strlen(textMessage) != 0){
                    load_Texture_Text(textMessage, inputTexture, font, white_color);
                    if(inputTexture->tWidth > inputMessage_rect.w - 12){
                        inputTexture->tWidth = inputMessage_rect.w - 12;
                    }
                    Render(inputTexture, 20, 729);
                }
                SDL_RenderPresent(renderer);
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
                        SDL_RenderPresent(renderer);
                        for(i = 0 ; i < 13; i++){
                            if(tmp >= i + 1){
                                GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                                load_Texture_Text(MessageLine[tmp - i - 1], showMessage, font, white_color);
                                Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                                SDL_RenderPresent(renderer);
                            }else{
                                break;
                            }
                        }
                    }else if(game_e.wheel.y < 0 && tmp < current_line){
                        tmp++;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        ClearRenderRect(showMessage_rect);
                        SDL_RenderPresent(renderer);
                        for(i = 0 ; i < 13; i++){
                            if(tmp >= i + 1){
                                GameTexture *showMessage = calloc(1, sizeof(GameTexture));
                                load_Texture_Text(MessageLine[tmp - i - 1], showMessage, font, white_color);
                                Render(showMessage, messLine_rect[13 - i].x, messLine_rect[13 - i].y);
                                SDL_RenderPresent(renderer);
                            }else{
                                break;
                            }
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}

int main(int argc, char** argv) {
    if(CreateWindowGame() == 0){
        exit(ERROR);
    }
    player_name = calloc(20, sizeof(char));
    strcpy(player_name, "Player1");
    Main_Screen();
    //int selection = 0;
    // do{
    //     selection = Main_Screen();
    //     selection = InGame_Screen(selection);
    // }while(selection == ESC);
    close_win();
    return SUCCEED;
}