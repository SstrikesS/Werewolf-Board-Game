#include "HostGame.h"
char *roomName;
char *num;

SDL_Surface *hSurface;

SDL_Rect rectBox = {230, 140, 600, 320};
SDL_Rect RoomlableBox = {410, 150, 0 ,0};
SDL_Rect NumlableBox = {385, 290, 0, 0};
SDL_Rect textField = {285, 212, 0, 0};
SDL_Rect textBox = {280, 210, 500, 60};
SDL_Rect lbutton = {400, 350, 0 ,0};
SDL_Rect rbutton = {600, 350, 0 ,0};
SDL_Rect numBox = {500, 350, 0 ,0};
SDL_Rect Okbutton = {370, 410, 0, 0};
SDL_Rect Backbutton = {610, 410, 0, 0};

TTF_Font *bloodfont;
TTF_Font *arialfont;

void RenderText(SDL_Renderer *renderer ,TTF_Font *font, char *text, SDL_Color color, SDL_Rect *textRect){
    SDL_Surface *sur = TTF_RenderText_Blended(font, text, color);
    TTF_SizeText(font, text, &textRect->w, &textRect->h);
    SDL_BlitSurface(sur, NULL, hSurface, textRect);
    SDL_FreeSurface(sur);
} 

void RenderHostScreen(SDL_Renderer *renderer){
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, hSurface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void getBox(SDL_Renderer *renderer){
    strcpy(roomName, "Player's Room");
    strcpy(num, "8");
    SDL_Color red_color = {255, 0, 0};
    bloodfont = TTF_OpenFont("bin/font/font.ttf", 50);
    arialfont = TTF_OpenFont("bin/font/arial.ttf", 50);

    SDL_FillRect(hSurface, &rectBox, SDL_MapRGB(hSurface->format, 0, 0, 0));
    RenderText(renderer, bloodfont, "Room Name", red_color, &RoomlableBox);
    SDL_FillRect(hSurface, &textBox, SDL_MapRGB(hSurface->format, 255, 255, 255));
    RenderText(renderer, arialfont, roomName, red_color, &textField);
    RenderText(renderer, bloodfont, "Max Players", red_color, &NumlableBox);
    RenderText(renderer, bloodfont, num, red_color, &numBox);
    RenderText(renderer, bloodfont, "<", red_color, &lbutton);
    RenderText(renderer, bloodfont, ">", red_color, &rbutton);
    RenderText(renderer, bloodfont, "OK", red_color, &Okbutton);
    RenderText(renderer, bloodfont, "Back", red_color, &Backbutton);
}

void HostGame(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window, CurrentPlayer *currUser){
    int i, extent, count = 0;
    SDL_Color yellow_color = {255, 255, 0};
    SDL_Color red_color = {255, 0, 0};
    SDL_Surface *sur = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event hEvent;
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    } 
    char *buffer = calloc(MAX_MESSAGE ,sizeof(char));
    roomName = calloc(MAX_RNAME, sizeof(char));
    num = calloc(4, sizeof(char));
    bloodfont = TTF_OpenFont("bin/font/font.ttf", 50);
    arialfont = TTF_OpenFont("bin/font/arial.ttf", 50);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    hSurface = IMG_Load("bin/img/hostgame.jpg");
    getBox(renderer);
    RenderHostScreen(renderer);
    SDL_RenderPresent(renderer);
    while(1){
        while(SDL_PollEvent(&hEvent)){
            switch (hEvent.type){
            case SDL_QUIT:
                memset(buffer, 0, sizeof(*buffer));
                buffer = GetMess(token, 0, EXIT_PACK);
                sendToServer(sockfd, server_addr, buffer);

                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                window = NULL;
                IMG_Quit();
                SDL_Quit();
                exit(0);
                break;
            case SDL_KEYDOWN:
                if(hEvent.key.keysym.sym == SDLK_BACKSPACE){
                    roomName[strlen(roomName) - 1] = '\0';
                    int check = count;
                    TTF_MeasureText(arialfont, roomName, textBox.w - 5, &extent, &count);
                    if(strlen(roomName) >= 0 && check != count){
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                        SDL_RenderFillRect(renderer, &textBox);
                        sur = TTF_RenderText_Blended(arialfont, roomName, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, sur);
                        TTF_SizeText(arialfont, roomName, &textField.w, &textField.h);
                        SDL_RenderCopy(renderer, texture, NULL, &textField);
                        SDL_RenderPresent(renderer); 
                    }
                }
                break;
            case SDL_TEXTINPUT:
                if(strlen(roomName) < MAX_RNAME){
                    strcat(roomName, hEvent.text.text);
                    int check = count;
                    TTF_MeasureText(arialfont, roomName, textBox.w - 5, &extent, &count);
                    if(strlen(roomName) >= 0 && check != count){
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                        SDL_RenderFillRect(renderer, &textBox);
                        sur = TTF_RenderText_Blended(arialfont, roomName, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, sur);
                        TTF_SizeText(arialfont, roomName, &textField.w, &textField.h);
                        SDL_RenderCopy(renderer, texture, NULL, &textField);
                        SDL_RenderPresent(renderer); 
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                if(check_mouse_pos(Okbutton) == 1){
                    sur = TTF_RenderText_Blended(bloodfont, "OK", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &Okbutton);
                }
                else{
                    sur = TTF_RenderText_Blended(bloodfont, "OK", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &Okbutton);
                }
                if(check_mouse_pos(Backbutton) == 1){
                    sur = TTF_RenderText_Blended(bloodfont, "Back", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &Backbutton);
                }else{
                    sur = TTF_RenderText_Blended(bloodfont, "Back", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &Backbutton);
                } 
                if(check_mouse_pos(lbutton) == 1){
                    sur = TTF_RenderText_Blended(bloodfont, "<", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &lbutton);
                }else{
                    sur = TTF_RenderText_Blended(bloodfont, "<", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &lbutton);
                }
                if(check_mouse_pos(rbutton) == 1){
                    sur = TTF_RenderText_Blended(bloodfont, ">", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &rbutton);
                }else{
                    sur = TTF_RenderText_Blended(bloodfont, ">", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, sur);
                    SDL_RenderCopy(renderer, texture, NULL, &rbutton);
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(Backbutton) == 1){
                    memset(buffer, 0, sizeof(*buffer));
                    buffer = GetMess(token, 0, EXIT_PACK);
                    sendToServer(sockfd, server_addr, buffer);
                    currUser->isHost = -1;
                    currUser->role = -1;
                    memset(currUser->room, 0, sizeof(*currUser->room));
                    currUser->id = -1;
                    free(num);
                    free(roomName);
                    free(buffer);
                    free(token);
                    free(hSurface);
                    return;
                }
                if(check_mouse_pos(lbutton) == 1){
                    int number = atoi(num);
                    if(number > 8){
                        number--;
                        memset(num, 0, sizeof(*num));
                        sprintf(num, "%d", number);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &numBox);
                        sur = TTF_RenderText_Blended(bloodfont, num, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, sur);
                        SDL_RenderCopy(renderer, texture, NULL, &numBox);
                    }
                }
                if(check_mouse_pos(rbutton) == 1){
                    int number = atoi(num);
                    if(number < 12){
                        number++;
                        memset(num, 0, sizeof(*num));
                        sprintf(num, "%d", number);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &numBox);
                        sur = TTF_RenderText_Blended(bloodfont, num, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, sur);
                        SDL_RenderCopy(renderer, texture, NULL, &numBox);
                    }
                }
                if(check_mouse_pos(Okbutton) == 1 && strlen(roomName) > 0){
                    strcpy(token[0], roomName);
                    strcpy(token[1], num);
                    memset(buffer, 0, sizeof(*buffer));
                    buffer = GetMess(token, 2, HOST_GAME);
                    sendToServer(sockfd, server_addr, buffer);
                    memset(buffer, 0, sizeof(*buffer));
                    ListenToServer(sockfd, server_addr, buffer);
                    free(token);
                    token = GetToken(buffer, 2);
                    enum pack_type type = (enum pack_type)atoi(token[0]);
                    if(type == SUCCEED_PACK){
                        printf("[+]Room's status -> Creating -> %s\n", token[1]);
                        strcpy(currUser->room, roomName);
                        currUser->isHost = 1;
                        free(token);
                        WaitingRoom(sockfd, server_addr, renderer, window, currUser);
                        return;
                    }else if(type == ERROR_PACK){
                        printf("[-]Room's status -> Creating -> %s\n", token[1]);
                    }else{
                        return;
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            default:
                break;
            }
        }
    }
}