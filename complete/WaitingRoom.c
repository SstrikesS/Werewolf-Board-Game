#include "WaitingRoom.h"

int playerCount = 0;
int roomMax = 0;
int isStart = -1;
int isNight = -1;
int isPlayerJoinning = -1;
int updateChatBox = -1;
char **groupChat;
int maxLine = 0;
int curr_line = 0;
char *Userchat;

SDL_Surface *wSurface;
SDL_Rect chatBox = {10, 260, 350, 290};
SDL_Rect *chatTextBox;
SDL_Rect listBox = {370, 100, 620, 430};
SDL_Rect settingBox = {10, 10, 350, 200};
SDL_Rect ClockRect = {370, 20, 80, 60};
SDL_Rect ClockText = {0, 0, 0, 0};
SDL_Rect NotiRect = {580, 20, 410, 60};
SDL_Rect NotiText = {0, 0, 0, 0};
typedef struct _SettingRow{
    SDL_Rect settingRow;
    char *text;
}SettingRow;

typedef struct _ChatRow{
    SDL_Rect pos;
    char *text;
}ChatRow;

typedef struct _PlayerBox{
    SDL_Rect pos;
    char *playerRole;
    SDL_Rect nameRect;
    SDL_Rect roleRect;
    SDL_Rect playerImG;
    char *playername;
    int player_id;
}PlayerBox;

typedef struct _Arg{
    SOCKET sockfd; 
    struct sockaddr_in server_addr;
    PlayerBox *pBox;
    CurrentPlayer *currUser;
    SDL_Renderer *renderer;
    SettingRow *Srow;
    enum pack_type type;
    char *buffer;
}Arg;

SDL_Rect ChatField = {10, 555, 350, 43};
SDL_Rect TextField = {15, 557, 330, 30};
SDL_Rect ButtonBack = {0, 0, 0, 0};
SDL_Rect ButtonStart = {0, 0, 0, 0};
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
void RenderInGame(SDL_Renderer *renderer){
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, wSurface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void rendertext(SDL_Renderer *renderer ,TTF_Font *font, char *text, SDL_Color color, SDL_Rect *textRect){
    SDL_Surface *sur = TTF_RenderText_Blended(font, text, color);
    TTF_SizeText(font, text, &textRect->w, &textRect->h);
    SDL_BlitSurface(sur, NULL, wSurface, textRect);
    SDL_FreeSurface(sur);
}

void GetRoom(SOCKET sockfd, struct sockaddr_in server_addr, PlayerBox *pBox, CurrentPlayer *currUser, char *buffer){
    int i, j;
    enum pack_type type;
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    } 
    token = GetToken(buffer, 3);
    type = (enum pack_type)atoi(token[0]);
    if(type == ROOM_INFO){
        playerCount = atoi(token[1]);
        roomMax = atoi(token[2]);
        memset(buffer, 0 , sizeof(*buffer));
        for(i = 0; i < 3; i++){
            memset(token[i], 0, sizeof(*token[i]));
        }
        ListenToServer(sockfd, server_addr, buffer);
        //printf("buffer2 = %s\n", buffer);
        token = GetToken(buffer, playerCount * 2 + 1);
        type = (enum pack_type)atoi(token[0]);
        if(type == ROOM_INFO){
            j = 1;
            for(i = 0; i < playerCount; i++){
                pBox[i].player_id = atoi(token[j++]);
                if(pBox[i].player_id == currUser->id){
                    strcpy(pBox[i].playername, "You");
                    j++;
                }else{
                    strcpy(pBox[i].playername, token[j++]);
                }
            }
        }else{
            
        }
    }
    free(token);
}

void GetPlayerList(SDL_Renderer *renderer, PlayerBox *pBox, CurrentPlayer *currUser, SettingRow *Srow){
    int i;
    char *img_link = calloc(100 ,sizeof(char));
    char *num = calloc(4, sizeof(char));
    SDL_FillRect(wSurface, &listBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_Surface *sur;
    TTF_Font * arialfont_10 = TTF_OpenFont("bin/font/arial.ttf", 10);
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 20);
    SDL_Color red_color = {255, 0, 0};
    for(i = 0; i < 12; i++){
        if(pBox[i].player_id != -1){
            memset(img_link, 0 ,sizeof(*img_link));
            memset(num, 0 ,sizeof(*num));
            strcat(img_link, "bin/img/pos");
            sprintf(num, "%d", i + 1);
            strcat(img_link, num);
            strcat(img_link, ".jpg");
            sur = IMG_Load(img_link);
            //printf("%s %d\n", pBox[i].playername, pBox[i].player_id);
            //printf("rect = %d %d %d %d\n", pBox[i].pos.x, pBox[i].pos.y, pBox[i].pos.w, pBox[i].pos.h);
            SDL_FillRect(wSurface, &pBox[i].pos, SDL_MapRGB(wSurface->format, 255, 255, 255));
            SDL_BlitSurface(sur, NULL, wSurface, &pBox[i].playerImG);
            rendertext(renderer, arialfont_10, pBox[i].playername, red_color, &pBox[i].nameRect);
            rendertext(renderer, arialfont_10, pBox[i].playerRole, red_color, &pBox[i].roleRect);
        }
    }
    SDL_FillRect(wSurface, &settingBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    strcpy(Srow[0].text, "Room name: ");
    strcat(Srow[0].text, currUser->room);
    strcpy(Srow[1].text, "Status: ");
    char *number = calloc(4, sizeof(char));
    sprintf(number, "%d", playerCount);
    strcat(Srow[1].text, number);
    strcat(Srow[1].text, "/");
    memset(number, 0, sizeof(*number));
    sprintf(number, "%d", roomMax);
    strcat(Srow[1].text, number);
    strcpy(Srow[2].text, "Time: None");
    strcpy(Srow[3].text, "Your role: None");
    strcpy(Srow[4].text, "Your status: None");
    free(number);

    for(i = 0; i < 5; i++){
        Srow[i].settingRow.x = 20;
        Srow[i].settingRow.y = 20 + i *40 ;
        rendertext(renderer, arialfont, Srow[i].text, red_color, &Srow[i].settingRow);
    }
    SDL_FreeSurface(sur);
}

void saveChat(CurrentPlayer *currUser, char *message){
    int extent, count, i = 0;
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 16);
    while(i < strlen(message) - 1){
        TTF_MeasureText(arialfont, &message[i], ChatField.w - 10, &extent, &count);
        strncat(groupChat[maxLine], &message[i], count);
        i += count;
        if(curr_line == maxLine++){
            curr_line++;
        }
        
    }
}

void GetChatBox(SDL_Renderer *renderer, int curr_line){
    int i;

    SDL_FillRect(wSurface, &chatBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_FillRect(wSurface, &ChatField, SDL_MapRGB(wSurface->format, 0, 0, 0));
    
    if(curr_line > 0){
        SDL_Surface *surface = NULL;
        SDL_Texture *texture = NULL;
        SDL_Color white_color = {255, 255, 255};
        TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 16);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer, &chatBox);

        if(curr_line >= 15){
            for(i = curr_line - 15 ; i < curr_line; i++){
                surface = TTF_RenderText_Blended_Wrapped(arialfont, groupChat[i], white_color, ChatField.w - 10);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                TTF_SizeText(arialfont, groupChat[i], &chatTextBox[i + 15 - curr_line].w, &chatTextBox[i + 15 - curr_line].h);
                SDL_RenderCopy(renderer, texture, NULL, &chatTextBox[i + 15 - curr_line]);
                SDL_BlitSurface(surface, NULL, wSurface, &chatTextBox[i + 15 - curr_line]);
            }
        }else{
            for(i = 0 ; i < curr_line; i++){
                surface = TTF_RenderText_Blended_Wrapped(arialfont, groupChat[i], white_color, ChatField.w - 10);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                TTF_SizeText(arialfont, groupChat[i], &chatTextBox[i].w, &chatTextBox[i].h);
                SDL_RenderCopy(renderer, texture, NULL, &chatTextBox[i]);
                SDL_BlitSurface(surface, NULL, wSurface, &chatTextBox[i]);
            }
        }
        
        // for(i = 14; i >= 0; i++){
        //     if(chatTextBox.w > (ChatField.w - 10) * i){
        //         chatTextBox.h = chatTextBox.h * (i + 1);
        //         chatTextBox.w = ChatField.w - 10;
        //         break;
        //     }     
        // }
        
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void GetUserChat(SDL_Renderer *renderer){
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    SDL_Color white_color = {255, 255, 255};
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 16);
    if(strlen(Userchat) > 0){
        surface = TTF_RenderText_Blended_Wrapped(arialfont, Userchat, white_color, ChatField.w - 10);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        TTF_SizeText(arialfont, Userchat, &TextField.w, &TextField.h);
        if(TextField.w > ChatField.w - 10){
            TextField.h = TextField.h * 2; 
            TextField.w = ChatField.w - 10;
        }
        SDL_RenderCopy(renderer, texture, NULL, &TextField);
    }
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void GetButton(SDL_Renderer *renderer, CurrentPlayer *currUser){
    ChatRow *row = calloc(7, sizeof(row));
    TTF_Font * bloodfont_25 = TTF_OpenFont("bin/font/font.ttf", 25);
    TTF_Font * bloodfont = TTF_OpenFont("bin/font/font.ttf", 40);
    TTF_Font * bloodfont_50 = TTF_OpenFont("bin/font/font.ttf", 50);
    SDL_Color red_color = {255, 0, 0};

    SDL_FillRect(wSurface, &ClockRect, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_FillRect(wSurface, &NotiRect, SDL_MapRGB(wSurface->format, 0, 0, 0));

    ButtonBack.x = 50;
    ButtonBack.y = 215;
    rendertext(renderer, bloodfont, "Exit", red_color, &ButtonBack);

    ClockText.x = 380;
    ClockText.y = 25;
    rendertext(renderer, bloodfont_50, "00", red_color, &ClockText);
    
    NotiText.x = 590;
    NotiText.y = 35;
    rendertext(renderer, bloodfont_25, "Waiting for other players", red_color, &NotiText);
    if(currUser->isHost == 1){
        ButtonStart.x = 150;
        ButtonStart.y = 215;
        rendertext(renderer, bloodfont, "Start", red_color, &ButtonStart);
    }
}

void *handleMess(void *argument){
    pthread_detach(pthread_self());
    int i;
    Arg *arg = (Arg *)argument;
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    }
    while(1){ 
        //printf("[+]Thread is waiting...\n");
        memset(buffer, 0 , sizeof(*buffer));
        ListenToServer(arg->sockfd, arg->server_addr, buffer);
        memset(arg->buffer, 0 , sizeof(*(arg->buffer)));
        strcpy(arg->buffer, buffer);
        //printf("buffer = %s\n", arg->buffer);
        arg->type = GetType(buffer);
        if(arg->type == ROOM_INFO){
            //printf("[+]Thread is executing\n");
            isPlayerJoinning = 1;
            GetRoom(arg->sockfd, arg->server_addr, arg->pBox, arg->currUser, buffer);
            GetPlayerList(arg->renderer, arg->pBox, arg->currUser, arg->Srow);
        }else if(arg->type == PUBLIC_MESSAGE_PACK){
            updateChatBox = 1;
            token = GetToken(arg->buffer, 2);
            saveChat(arg->currUser, token[1]);
            if(curr_line == maxLine){
                GetChatBox(arg->renderer, maxLine);
            }
            for(i = 0; i < 2; i++){
                memset(token[i], 0 , sizeof(*token[i]));
            }
        }else if(arg->type == START_GAME){
            isStart = 1;
            token = GetToken(arg->buffer, 2);
            arg->currUser->role = (enum Role)atoi(token[1]);
            printf("Your role = %d\n", (int)arg->currUser->role);
        }
    }
    return NULL;
}

void WaitingRoom(SOCKET sockfd, struct sockaddr_in server_addr, SDL_Renderer *renderer, SDL_Window *window,  CurrentPlayer *currUser){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    int i, j;
    pthread_t tid;
    groupChat = calloc(1, sizeof(char *));
    for(i = 0; i < 1000; i++){
        groupChat[i] = calloc(100, sizeof(char));
    }
    Userchat = calloc(MAX_MESSAGE, sizeof(char));
    chatTextBox = calloc(15, sizeof(SDL_Rect));
    for(i = 0; i < 15; i++){
        chatTextBox[i].x = 15;
        chatTextBox[i].y = 265 + 18 * i;
    }
    // strcpy(Userchat, "fhsa;fjsa;dfnas;tuf;osrhwerwprwfdpafxvnpxiuvhxiourwfdpyyafxvnpxiuvhxioyyyuhvozxihv8zxc7v8xcvxv");
    // enum pack_type type;
    char **token = calloc(200, sizeof(char *));
    for(i = 0; i < 200; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    } 
    char *SendBuffer = calloc(MAX_MESSAGE ,sizeof(char));
    char *ListenBuffer = calloc(MAX_MESSAGE ,sizeof(char));
    PlayerBox *pBox = calloc(12, sizeof(PlayerBox));
    SettingRow *Srow = calloc(5, sizeof(SettingRow));
    SDL_Surface * surface = NULL;
    SDL_Texture * texture = NULL;
    SDL_Color red_color = {255, 0, 0};
    SDL_Color yellow_color = {255, 255, 0};
    TTF_Font * bloodfont = TTF_OpenFont("bin/font/font.ttf", 40);
    // TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 16);
    for(i = 0; i < 5; i++){
        Srow[i].text = calloc(MAX_MESSAGE, sizeof(char));
    }
    j = 0;
    for(i = 0; i < 12; i++){
        pBox[i].playerRole = calloc(MAX_NAME, sizeof(char));
        pBox[i].playername = calloc(MAX_NAME, sizeof(char));
        strcpy(pBox[i].playerRole, "???");
        pBox[i].player_id = -1;

        pBox[i].pos.w = 80; 
        pBox[i].nameRect.w = pBox[i].roleRect.w = pBox[i].playerImG.w = 70;
        pBox[i].pos.h = 150;
        pBox[i].nameRect.h = pBox[i].roleRect.h = 25;
        pBox[i].playerImG.h = 80;
        if(i > 5){
            j = 1;
        }
        pBox[i].pos.x = 390 + 100 * (i - (j * 6));
        pBox[i].nameRect.x = pBox[i].roleRect.x = pBox[i].playerImG.x = 395 + 100 * (i - (j * 6)); 
        pBox[i].pos.y = 120 + j * 200;
        pBox[i].nameRect.y = 125 + j * 200;
        pBox[i].roleRect.y = 240 + j * 200;
        pBox[i].playerImG.y = 155 + j * 200;
    }

    Arg *arg = calloc(1, sizeof(Arg));
    arg->sockfd = sockfd;
    arg->server_addr = server_addr;
    arg->currUser = currUser;
    arg->pBox = pBox;
    arg->renderer = renderer;
    arg->Srow = Srow;
    arg->buffer = ListenBuffer;
    wSurface = IMG_Load("bin/img/ingame.jpg");

    pthread_create(&tid, NULL, handleMess, (void *)arg);

    SDL_FillRect(wSurface, &chatBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_FillRect(wSurface, &ChatField, SDL_MapRGB(wSurface->format, 0, 0, 0));

    //GetUserChat(renderer);
    GetButton(renderer, currUser);

    SDL_Delay(500);
    RenderInGame(renderer);
    SDL_RenderPresent(renderer);
    isPlayerJoinning = -1;
    SDL_Event gEvent;
    while(1){
        if(isPlayerJoinning == 1){
            SDL_Delay(500);
            RenderInGame(renderer);
            SDL_RenderPresent(renderer);
            isPlayerJoinning = -1;
        }
        if(updateChatBox == 1){
            SDL_Delay(100);
            SDL_RenderPresent(renderer);
            updateChatBox = -1;
        }
        while(SDL_PollEvent(&gEvent)){
            switch (gEvent.type){
            case SDL_QUIT:
                j = pthread_cancel(tid);
            
                memset(SendBuffer, 0, sizeof(*SendBuffer));
                SendBuffer = GetMess(token, 0, EXIT_PACK);
                sendToServer(sockfd, server_addr, SendBuffer);

                SDL_FreeSurface(wSurface);
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                window = NULL;
                //TTF_Quit();
                IMG_Quit();
                SDL_Quit();
                exit(0);
                break;
            case SDL_MOUSEMOTION:
                if(check_mouse_pos(ButtonBack) == 1 && isStart == -1){
                    surface = TTF_RenderText_Blended(bloodfont, "Exit", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, texture, NULL, &ButtonBack);
                }else{
                    surface = TTF_RenderText_Blended(bloodfont, "Exit", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, texture, NULL, &ButtonBack);
                }
                if(check_mouse_pos(ButtonStart) == 1 && isStart == -1){
                    surface = TTF_RenderText_Blended(bloodfont, "Start", yellow_color);
                    texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, texture, NULL, &ButtonStart);
                }else{
                    surface = TTF_RenderText_Blended(bloodfont, "Start", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, texture, NULL, &ButtonStart);
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(ButtonStart) == 1 && isStart == -1){
                    memset(SendBuffer, 0, sizeof(*SendBuffer));
                    SendBuffer = GetMess(token , 0, START_GAME);
                    sendToServer(sockfd, server_addr, SendBuffer);

                    while(strlen(arg->buffer) > 0){
                        token = GetToken(arg->buffer, 2);
                        if(arg->type == SUCCEED_PACK){
                            printf("[+]Server :%s\n", token[1]);
                        }
                        else if(arg->type == ERROR_PACK){
                            printf("[-]Server :%s\n", token[1]);
                        }else{
                            return;
                        }
                        for(i = 0; i < 2; i++){
                            memset(token[i], 0, sizeof(*token[i]));
                        }
                        break;
                    }            
                }
                if(check_mouse_pos(ButtonBack) == 1){
                    pthread_cancel(tid);
                    return;
                }
                break;
            case SDL_MOUSEWHEEL:
                if(check_mouse_pos(chatBox) == 1){
                    if(gEvent.wheel.y > 0 && curr_line < maxLine){
                        curr_line++;
                        GetChatBox(renderer, curr_line);
                    }else if(gEvent.wheel.y < 0 && curr_line >= 15){
                        curr_line--;
                        GetChatBox(renderer, curr_line);
                    }else{
                        break;
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_KEYDOWN:
                if(gEvent.key.keysym.sym == SDLK_BACKSPACE){
                    Userchat[strlen(Userchat) - 1] = '\0';
                    if(strlen(Userchat) >= 0){
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &ChatField);
                        GetUserChat(renderer);  
                    }
                }
                if(gEvent.key.keysym.sym == SDLK_KP_ENTER || gEvent.key.keysym.sym == SDLK_RETURN){
                    if(strlen(Userchat) > 0){

                        strcpy(token[0], Userchat);
                        memset(SendBuffer, 0, sizeof(*SendBuffer));
                        SendBuffer = GetMess(token, 1, PUBLIC_MESSAGE_PACK);
                        memset(token[0], 0 ,sizeof(*token[0]));
                        sendToServer(sockfd, server_addr, SendBuffer);
                        

                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &ChatField);
                        memset(SendBuffer, 0, sizeof(*SendBuffer));
                        strcat(SendBuffer, "You: ");
                        strcat(SendBuffer, Userchat);

                        // saveChat(currUser, SendBuffer);
                        // GetChatBox(renderer);

                        memset(SendBuffer, 0, sizeof(*SendBuffer));
                        memset(Userchat, 0 , sizeof(*Userchat));
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_TEXTINPUT:
                if(strlen(Userchat) < 100){
                    strcat(Userchat, gEvent.text.text);
                    if(strlen(Userchat) >= 0){
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &ChatField);
                        GetUserChat(renderer);
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




