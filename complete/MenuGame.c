#include "MenuGame.h"
#include <pthread.h>
#include <windows.h>

char *list[] = {"Player Name", "Host Game", "Join Game", "Exit"};

SDL_Surface *gSurface;
SDL_Rect *listRect;
SDL_Rect textbox = {700, 180, 250, 55};
SDL_Rect textRect = {710, 175, 0, 0};
TTF_Font *menuFont;
TTF_Font *playerFont;
CurrentPlayer *currUser;

void *sendToPingServer(){
    pthread_detach(pthread_self());
    char *buffer = calloc(4, sizeof(char));
    sprintf(buffer, "%d", currUser->id);
    printf("%s\n", buffer);
    struct sockaddr_in pingServer;
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ZeroMemory(&pingServer, sizeof(pingServer));
    pingServer.sin_family = AF_INET;
	pingServer.sin_port = htons(PORT + 1);
	pingServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDR);
    while(currUser->id != -1){
        sendToServer(sockfd, pingServer, buffer);
        Sleep(1000);
    }
    return NULL;
}

void GetCurrUser(){
    currUser = calloc(1, sizeof(CurrentPlayer));
    currUser->name = calloc(MAX_NAME, sizeof(char));
    strcpy(currUser->name, "Player");
    currUser->room = calloc(MAX_RNAME, sizeof(char));
    currUser->id = -1;
    currUser->isHost = -1;
}
void SetTextBox(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *playerfont){
    SDL_RenderFillRect(renderer, &textbox);
    SDL_Color red_color = {255, 0, 0};
    SDL_Surface *surface = TTF_RenderText_Blended(playerFont, currUser->name, red_color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    TTF_SizeText(playerFont, currUser->name, &textRect.w, &textRect.h);
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void GetMenuList(SDL_Renderer *renderer){
    int i;
    SDL_Surface *surface = NULL;
    SDL_Texture *listTexture = NULL;
    SDL_Color red_color = {255, 0, 0};
    listRect = calloc(4, sizeof(SDL_Rect));
    menuFont = TTF_OpenFont("bin/font/font.ttf", 50);
    playerFont = TTF_OpenFont("bin/font/arial.ttf", 50); 
    
    for(i = 0; i < 4; i++){
        surface = TTF_RenderText_Blended(menuFont, list[i], red_color);
        TTF_SizeText(menuFont, list[i], &listRect[i].w, &listRect[i].h);
        if(i != 0){
            listRect[i].x = 700;
            listRect[i].y = (450 + listRect[i].h *(i * 4 - 2)) / 2;
            SDL_BlitSurface(surface, NULL, gSurface, &listRect[i]);
        }else{
            listRect[i].x = 700;
            listRect[i].y = 100;
            SDL_BlitSurface(surface, NULL, gSurface, &listRect[i]);
            SDL_FillRect(gSurface, &textbox, SDL_MapRGB(gSurface->format, 255, 255, 255));
            surface = TTF_RenderText_Blended(playerFont, currUser->name, red_color);
            TTF_SizeText(playerFont, currUser->name, &textRect.w, &textRect.h);
            SDL_BlitSurface(surface, NULL, gSurface, &textRect);
        }
    }
    SDL_DestroyTexture(listTexture);
    SDL_FreeSurface(surface);
}

void RenderMenuScreen(SDL_Renderer *renderer){
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, gSurface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void MainMenu(SDL_Window *window, SDL_Renderer *renderer){
    int i, extent, count = 0;
    SDL_Surface *surface = NULL;
    SDL_Texture *listTexture = NULL;
    menuFont = TTF_OpenFont("bin/font/font.ttf", 50);
    SDL_Event menuE;
    SDL_Color yellow_color = {255, 255, 0};
    SDL_Color red_color = {255, 0, 0};
    pthread_t tid;
    GetCurrUser();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    gSurface = IMG_Load("bin/img/gbackgroud.jpg");
    GetMenuList(renderer);
    RenderMenuScreen(renderer);
    SDL_RenderPresent(renderer);
    
    while(1){
        while(SDL_PollEvent(&menuE)){
            switch (menuE.type){
            case SDL_QUIT:
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                window = NULL;
                TTF_Quit();
                IMG_Quit();
                SDL_Quit();
                exit(0);
                break;
            case SDL_MOUSEMOTION:
                for(i = 1; i < 4; i++){
                    if(check_mouse_pos(listRect[i]) == 1){
                        surface = TTF_RenderText_Blended(menuFont, list[i], yellow_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);   
                        SDL_RenderCopy(renderer, listTexture, NULL, &listRect[i]);
                        SDL_RenderPresent(renderer);
                    }else{
                        surface = TTF_RenderText_Blended(menuFont, list[i], red_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_RenderCopy(renderer, listTexture, NULL, &listRect[i]);
                        SDL_RenderPresent(renderer);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(listRect[1]) == 1 && strlen(currUser->name) > 0){
                    SOCKET sockfd = setupSocket();
                    struct sockaddr_in server_addr = setupServerAddr();
                    int check = LoginToServer(sockfd, server_addr, currUser->name);
                    currUser->id = check;
                    // HostGame(sockfd, server_addr, renderer, window, currUser);
                    // RenderMenuScreen(renderer);
                    // SDL_RenderPresent(renderer);
                    if(check == ERROR_RETURN){
                        
                    }else{
                        pthread_create(&tid, NULL, sendToPingServer, NULL);
                        HostGame(sockfd, server_addr, renderer, window, currUser);
                        RenderMenuScreen(renderer);
                        SDL_RenderPresent(renderer);
                        printf("[+]Log out!\n");
                    }
                }else if(check_mouse_pos(listRect[2]) == 1){
                    SOCKET sockfd = setupSocket();
                    struct sockaddr_in server_addr = setupServerAddr();
                    int check = LoginToServer(sockfd, server_addr, currUser->name);
                    currUser->id = check;
                    // JoinGame(sockfd, server_addr, renderer, window, currUser);
                    // RenderMenuScreen(renderer);
                    // SDL_RenderPresent(renderer);
                    if(check == ERROR_RETURN){
                        
                    }else{
                        pthread_create(&tid, NULL, sendToPingServer, NULL);
                        JoinGame(sockfd, server_addr, renderer, window, currUser);
                        RenderMenuScreen(renderer);
                        SDL_RenderPresent(renderer);
                        printf("[+]Log out!\n");
                    }
                }else if(check_mouse_pos(listRect[3]) == 1){
                    SDL_DestroyWindow(window);
                    SDL_DestroyRenderer(renderer);
                    window = NULL;
                    TTF_Quit();
                    IMG_Quit();
                    SDL_Quit();
                    exit(0);
                }
                break;
            case SDL_KEYDOWN:
                if(menuE.key.keysym.sym == SDLK_BACKSPACE){
                    currUser->name[strlen(currUser->name) - 1] = '\0';
                    int check = count;
                    TTF_MeasureText(playerFont, currUser->name, textbox.w - 5, &extent, &count);
                    if(strlen(currUser->name) >= 0 && check != count){
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                        SetTextBox(window, renderer, playerFont);
                        SDL_RenderPresent(renderer); 
                    }
                }
                break;
            case SDL_TEXTINPUT:
                if(strlen(currUser->name) < MAX_NAME){
                    strcat(currUser->name, menuE.text.text);
                    int check = count;
                    TTF_MeasureText(playerFont, currUser->name, textbox.w - 5, &extent, &count);
                    if(strlen(currUser->name) >= 0 && check != count){
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                        SetTextBox(window, renderer, playerFont);
                        SDL_RenderPresent(renderer); 
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}