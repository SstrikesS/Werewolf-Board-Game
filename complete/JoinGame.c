#include "JoinGame.h"

typedef struct _Row{
    char *roomName;
    char *playerCount;
    char *maxPlayer;
    SDL_Rect pos;
    SDL_Rect roomNameBox;
    SDL_Rect statusBox;
}Row;

SDL_Surface *jSurface;
SDL_Rect boxList = {200, 50, 600, 500};
SDL_Rect joinButton = {240, 490, 0, 0};
SDL_Rect backButton = {620, 490, 0, 0};
SDL_Rect BoxLable = {395, 60, 0, 0};
SDL_Rect refreshButton = {400, 490, 0, 0};
int roomcount;

void GetRoomList(SOCKET sockfd, struct sockaddr_in server_addr, Row *roomlist, char **token){
    int i, j;
    char *buffer = calloc(MAX_MESSAGE ,sizeof(char));

    memset(buffer, 0, sizeof(*buffer));
    buffer = GetMess(token, 0, JOIN_GAME);
    sendToServer(sockfd, server_addr, buffer);

    memset(buffer, 0, sizeof(*buffer));
    ListenToServer(sockfd, server_addr, buffer);

    memset(token, 0, sizeof(*token[0]));
    token = GetToken(buffer, 2);
    
    enum pack_type type = (enum pack_type)atoi(token[0]);
    if(type == JOIN_GAME){
        roomcount = atoi(token[1]);
        printf("[+]Room founds: %d rooms!\n", roomcount);
    }else{
        printf("[-]Can't get room list from server!\n");
        return;
    }
    for(i = 0; i < roomcount; i++){
        roomlist[i].roomName = calloc(MAX_RNAME , sizeof(char));
        roomlist[i].playerCount = calloc(4 , sizeof(char));
        roomlist[i].maxPlayer = calloc(4 , sizeof(char));
    }

    memset(buffer, 0, sizeof(*buffer));
    ListenToServer(sockfd, server_addr, buffer);
    memset(token, 0, sizeof(*token[0]));
    token = GetToken(buffer, roomcount * 3 + 1);
    printf("token = %s!\n", token[1]);
    j = 1;
    for(i = 0; i < roomcount; i++){
        strcpy(roomlist[i].roomName, token[j++]);
        strcpy(roomlist[i].maxPlayer, token[j++]);
        strcpy(roomlist[i].playerCount, token[j++]);
        roomlist[i].roomName[strlen(roomlist[i].roomName)] = '\0';
        roomlist[i].maxPlayer[strlen(roomlist[i].maxPlayer)] = '\0';
        roomlist[i].playerCount[strlen(roomlist[i].playerCount)] = '\0';
    }
    
    for(i = 0; i < roomcount *3 + 1; i++){
        memset(token[i], 0, sizeof(*token[i]));
    }
    free(buffer);
}

void RenderJoinScreen(SDL_Renderer *renderer){
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, jSurface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void Rendertext(SDL_Renderer *renderer ,TTF_Font *font, char *text, SDL_Color color, SDL_Rect *textRect){
    SDL_Surface *sur = TTF_RenderText_Blended(font, text, color);
    TTF_SizeText(font, text, &textRect->w, &textRect->h);
    SDL_BlitSurface(sur, NULL, jSurface, textRect);
    SDL_FreeSurface(sur);
}

void getListBox(SDL_Renderer *renderer, Row *roomlist){
    int i;
    char *status = calloc(MAX_RNAME, sizeof(char));
    SDL_Color red_color = {255, 0, 0};
    TTF_Font *bloodfont = TTF_OpenFont("bin/font/font.ttf", 50);
    TTF_Font *arialfont = TTF_OpenFont("bin/font/arial.ttf", 26);

    SDL_FillRect(jSurface, &boxList, SDL_MapRGB(jSurface->format, 0, 0, 0));

    Rendertext(renderer, bloodfont, "Room List", red_color, &BoxLable);

    for(i = 0; i < roomcount; i++){
        roomlist[i].pos.x = 220;
        roomlist[i].pos.y = 120 + i * 40;
        roomlist[i].pos.w = 560;
        roomlist[i].pos.h = 30;
        SDL_FillRect(jSurface, &roomlist[i].pos, SDL_MapRGB(jSurface->format, 255, 255, 255));
        
        roomlist[i].roomNameBox.x = 240;
        roomlist[i].roomNameBox.y = 120 + i * 40;
        Rendertext(renderer, arialfont, roomlist[i].roomName, red_color, &roomlist[i].roomNameBox);

        memset(status, 0, sizeof(*status));
        strcat(status, roomlist[i].playerCount);
        strcat(status, "/");
        strcat(status, roomlist[i].maxPlayer);

        roomlist[i].statusBox.x = 600;
        roomlist[i].statusBox.y = 120 + i * 40;
        Rendertext(renderer, arialfont, status, red_color, &roomlist[i].statusBox);
    }
    Rendertext(renderer, bloodfont, "Join", red_color, &joinButton);
    Rendertext(renderer, bloodfont, "Refresh", red_color, &refreshButton);
    Rendertext(renderer, bloodfont, "Back", red_color, &backButton);
    free(status);
}

void JoinGame(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window, CurrentPlayer *currUser){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    int i, row = -1;
    SDL_Event JEvent;
    SDL_Surface *surface = NULL;
    SDL_Texture *listTexture = NULL;
    TTF_Font *bloodfont = TTF_OpenFont("bin/font/font.ttf", 50);
    TTF_Font *arialfont = TTF_OpenFont("bin/font/arial.ttf", 26);
    SDL_Color red_color = {255, 0, 0};
    SDL_Color blue_color = {0, 0, 255};
    SDL_Color yellow_color = {255, 255, 0};
    Row *roomlist = calloc(roomcount, sizeof(Row));
    enum pack_type type;
    char **token =  makeCleanToken();
    char *buffer = calloc(MAX_MESSAGE ,sizeof(char));
    char *status = calloc(MAX_RNAME, sizeof(char));
    GetRoomList(sockfd, server_addr, roomlist, token);
    jSurface = IMG_Load("bin/img/hostgame.jpg");
    getListBox(renderer, roomlist);
    RenderJoinScreen(renderer);
    SDL_RenderPresent(renderer);
    
    while(1){
        while(SDL_PollEvent(&JEvent)){
            switch (JEvent.type){
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
            case SDL_MOUSEMOTION:
                for(i = 0; i < roomcount; i++){
                    if(check_mouse_pos(roomlist[i].pos) == 1){
                        memset(status, 0, sizeof(*status));
                        strcat(status, roomlist[i].playerCount);
                        strcat(status, "/");
                        strcat(status, roomlist[i].maxPlayer);

                        surface = TTF_RenderText_Blended(arialfont, roomlist[i].roomName, blue_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].roomNameBox);

                        surface = TTF_RenderText_Blended(arialfont, status, blue_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].statusBox);
                    }else{
                        memset(status, 0, sizeof(*status));
                        strcat(status, roomlist[i].playerCount);
                        strcat(status, "/");
                        strcat(status, roomlist[i].maxPlayer);

                        surface = TTF_RenderText_Blended(arialfont, roomlist[i].roomName, red_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].roomNameBox);

                        surface = TTF_RenderText_Blended(arialfont, status, red_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].statusBox);
                    }
                }
                if(check_mouse_pos(joinButton) == 1){
                    surface = TTF_RenderText_Blended(bloodfont, "Join", yellow_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &joinButton);
                }else{
                    surface = TTF_RenderText_Blended(bloodfont, "Join", red_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &joinButton);
                }
                if(check_mouse_pos(refreshButton) == 1){
                    surface = TTF_RenderText_Blended(bloodfont, "Refresh", yellow_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &refreshButton);
                }else{
                    surface = TTF_RenderText_Blended(bloodfont, "Refresh", red_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &refreshButton);
                }
                if(check_mouse_pos(backButton) == 1){
                    surface = TTF_RenderText_Blended(bloodfont, "Back", yellow_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &backButton);
                }else{
                    surface = TTF_RenderText_Blended(bloodfont, "Back", red_color);
                    listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, listTexture, NULL, &backButton);
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                for(i = 0; i < roomcount; i++){
                    if(check_mouse_pos(roomlist[i].pos) == 1){
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0);
                        SDL_RenderFillRect(renderer, &roomlist[i].pos);
                        memset(status, 0, sizeof(*status));
                        strcat(status, roomlist[i].playerCount);
                        strcat(status, "/");
                        strcat(status, roomlist[i].maxPlayer);
                        
                        surface = TTF_RenderText_Blended(arialfont, roomlist[i].roomName, blue_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].roomNameBox);

                        surface = TTF_RenderText_Blended(arialfont, status, blue_color);
                        listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[i].statusBox);
                        if(row != -1 && row != i){
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                            SDL_RenderFillRect(renderer, &roomlist[row].pos);
                            memset(status, 0, sizeof(*status));
                            strcat(status, roomlist[row].playerCount);
                            strcat(status, "/");
                            strcat(status, roomlist[row].maxPlayer);

                            surface = TTF_RenderText_Blended(arialfont, roomlist[row].roomName, red_color);
                            listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                            SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[row].roomNameBox);

                            surface = TTF_RenderText_Blended(arialfont, status, red_color);
                            listTexture = SDL_CreateTextureFromSurface(renderer, surface);    
                            SDL_RenderCopy(renderer, listTexture, NULL, &roomlist[row].statusBox);
                        }
                        row = i;
                    }
                }
                if(check_mouse_pos(backButton) == 1){
                    memset(buffer, 0, sizeof(*buffer));
                    buffer = GetMess(token, 0, EXIT_PACK);
                    sendToServer(sockfd, server_addr, buffer);
                    currUser->isHost = -1;
                    currUser->role = -1;
                    memset(currUser->room, 0, sizeof(*currUser->room));
                    currUser->id = -1;
                    free(buffer);
                    free(token);
                    free(status);
                    SDL_FreeSurface(jSurface);
                    SDL_DestroyTexture(listTexture);
                    SDL_FreeSurface(surface);
                    return;
                }
                if(check_mouse_pos(refreshButton) == 1){
                    GetRoomList(sockfd, server_addr, roomlist, token);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    jSurface = IMG_Load("bin/img/hostgame.jpg");
                    getListBox(renderer, roomlist);
                    RenderJoinScreen(renderer);
                }
                if(check_mouse_pos(joinButton) == 1){
                    if(row != -1){
                        printf("row = %d and room = %s and player = %s!\n", row, roomlist[row].roomName, currUser->name);
                        memset(buffer, 0, sizeof(*buffer));
                        printf("%d %d!\n", strlen(token[0]), strlen(token[1]));
                        strcpy(token[0], currUser->name);
                        strcpy(token[1], roomlist[row].roomName);
                        printf("token = %s and %s!\n", token[0], token[1]);
                        buffer = GetMess(token, 2, JOIN_ROOM);
                        printf("buffer = %s\n", buffer);
                        sendToServer(sockfd, server_addr, buffer);
                        memset(buffer, 0, sizeof(*buffer));
                        ListenToServer(sockfd, server_addr, buffer);
                        printf("buffer = %s\n", buffer);
                        memset(token, 0, sizeof(*token[0]));
                        token = GetToken(buffer, 2);
                        type = (enum pack_type)atoi(token[0]);
                        if(type == SUCCEED_PACK){
                            printf("[+]Joining Room -> %s\n", token[1]);
                            strcpy(currUser->room, roomlist[row].roomName);
                            memset(token, 0, sizeof(*token[0]));
                            WaitingRoom(sockfd, server_addr, renderer, window, currUser);
                            return;
                        }else if(type == ERROR_PACK){
                            printf("[-]Joining Room -> %s\n", token[1]);
                            for(i = 0; i < 2; i++){
                                memset(token[i], 0 ,sizeof(*token[i]));
                            }
                        }
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