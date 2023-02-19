#include "WaitingRoom.h"

int playerCount = 0;
int roomMax = 0;
int isStart = -1;
int isNight = -1;
int isYourTurn = -1;
int isDead = 0;
int isPlayerJoinning = -1;
int updateChatBox = -1;
char **groupChat;
int maxLine = 0;
int curr_line = 0;
int timeout = 0;
char *Userchat;
int targetBox = -1;
int lastBox = -1;

SDL_Surface *wSurface;
SDL_Rect chatBox = {10, 260, 350, 290};
SDL_Rect *chatTextBox;
SDL_Rect listBox = {370, 100, 620, 430};
SDL_Rect settingBox = {10, 10, 350, 200};
SDL_Rect ClockRect = {370, 20, 80, 60};
SDL_Rect ClockText = {0, 0, 0, 0};
SDL_Rect NotiRect = {580, 20, 410, 60};
SDL_Rect NotiText = {0, 0, 0, 0};
SDL_Rect SkipRect = {470, 25, 0, 0};
SDL_Rect VoteRect = {625, 540, 0, 0};
SDL_Rect Skill_1Rect = {400, 540, 0, 0};
SDL_Rect Skill_2Rect = {850, 540, 0, 0};

char *skill_1;
char *skill_2;
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
    char *img_link;
    int isDead;
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
    char **token;
    int timeout;
}Arg;

PlayerBox *currBox;
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

void *time_countdown(void *argument){
    pthread_detach(pthread_self());
    Arg *arg = (Arg *)argument;
    char *timetext = calloc(3, sizeof(char));
    char *tmp = calloc(2, sizeof(char));
    SDL_Color red_color = {255, 0, 0};
    SDL_Color yellow_color = {255, 255, 0};
    SDL_Surface *sur;
    SDL_Texture *texture;
    TTF_Font *font = TTF_OpenFont("bin/font/font.ttf", 40);
    TTF_Font *font_25 = TTF_OpenFont("bin/font/font.ttf", 25);
    SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
    do{
        SDL_Delay(1000);
        memset(timetext, 0, sizeof( *timetext));
        
        if(arg->timeout >= 10){
            sprintf(timetext, "%d", arg->timeout);
            sur = TTF_RenderText_Blended(font, timetext, yellow_color);
        }else{
            sprintf(tmp, "%d", arg->timeout);
            strcat(timetext, "0");
            strcat(timetext, tmp);
            sur = TTF_RenderText_Blended(font, timetext, red_color);
        }
        SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(arg->renderer, &ClockRect);
        texture = SDL_CreateTextureFromSurface(arg->renderer, sur);
        SDL_RenderCopy(arg->renderer, texture, NULL, &ClockText);
        SDL_RenderPresent(arg->renderer);
        arg->timeout--;
    }while (arg->timeout >= 0);
    SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(arg->renderer, &NotiRect);
    sur = TTF_RenderText_Blended(font_25, "Waiting for other players!", red_color);
    TTF_SizeText(font_25, "Waiting for other players!", &NotiText.w, &NotiText.h);
    texture = SDL_CreateTextureFromSurface(arg->renderer, sur);    
    SDL_RenderCopy(arg->renderer, texture, NULL, &NotiText);
    SDL_RenderPresent(arg->renderer);
    if(strlen(arg->token[0]) == 0){
        strcpy(arg->token[0], "-1");
        strcpy(arg->token[1], "0");
    }
    char *buffer = calloc(10, sizeof(char));
    buffer = GetMess(arg->token, 2, PLAYER_TARGET);
    cleanToken(arg->token, 2);
    sendToServer(arg->sockfd, arg->server_addr, buffer);
    free(buffer);
    return NULL;
}

void UpdateUI(SDL_Renderer* renderer, CurrentPlayer *currUser, SettingRow *Srow, Arg *arg){
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 20);
    TTF_Font * bloodfont_50 = TTF_OpenFont("bin/font/font.ttf", 50);
    TTF_Font * arialfont_10 = TTF_OpenFont("bin/font/arial.ttf", 10);
    SDL_Color red_color = {255, 0, 0};
    int i;
    SDL_Surface *sur = NULL;
    SDL_Texture *texture = NULL;
    char *vote = "Vote";
    char *skip = "Skip";
    skill_1 = calloc(30, sizeof(char));
    skill_2 = calloc(30, sizeof(char));
    memset(Srow[3].text, 0, sizeof(*Srow[3].text));
    memset(Srow[2].text, 0, sizeof(*Srow[2].text));
    memset(Srow[2].text, 0, sizeof(*Srow[4].text));
    strcpy(Srow[3].text, "Your role: ");
    strcpy(Srow[2].text, "Time: Night");
    strcpy(Srow[4].text, "Your status: Alive");
    switch (currUser->role){
    case WEREWOLF:
        strcpy(skill_1 , "Vote Kill");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);

        strcat(Srow[3].text, "WereWolf");
        strcpy(currBox->playerRole, "WereWolf");
        VoteRect.x = 850;
        break;
    case WITCH:
        strcpy(skill_1 , "Protect");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        Skill_1Rect.x = 370;
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
        strcpy(skill_2 , "Poision");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_2, red_color);
        TTF_SizeText(bloodfont_50, skill_2, &Skill_2Rect.w, &Skill_2Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_2Rect);
        strcat(Srow[3].text, "Witch");
        strcpy(currBox->playerRole, "Witch");
        break;
    case PROTECTER:
        strcpy(skill_1 , "Protect");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
        strcat(Srow[3].text, "Protecter");
        strcpy(currBox->playerRole, "Protecter");
        VoteRect.x = 850;
        break;
    case SEER:
        strcpy(skill_1 , "Predict");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
        strcat(Srow[3].text, "Seer");
        strcpy(currBox->playerRole, "Seer");
        VoteRect.x = 850;
        break;
    case HUNTER:
        strcpy(skill_1 , "Hunt");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
        strcat(Srow[3].text, "Hunter");
        strcpy(currBox->playerRole, "Hunter");
        VoteRect.x = 850;
        break;
    case VILLAGE:
        strcpy(skill_1 , "Sleep");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
        TTF_SizeText(bloodfont_50, skill_1, &Skill_1Rect.w, &Skill_1Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        Skill_1Rect.x = 370;
        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
        strcpy(skill_2 , "Eat");

        sur = TTF_RenderText_Blended(bloodfont_50, skill_2, red_color);
        TTF_SizeText(bloodfont_50, skill_2, &Skill_2Rect.w, &Skill_2Rect.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Skill_2Rect);
        strcat(Srow[3].text, "Village");
        strcpy(currBox->playerRole, "Village");
        break;
    default:
        break;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &currBox->roleRect);
    sur = TTF_RenderText_Blended(arialfont, currBox->playerRole, red_color);
    TTF_SizeText(arialfont_10, currBox->playerRole, &currBox->roleRect.w, &currBox->roleRect.h);
    texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopy(renderer, texture, NULL, &currBox->roleRect);

    sur = TTF_RenderText_Blended(bloodfont_50, vote, red_color);
    TTF_SizeText(bloodfont_50, vote, &VoteRect.w, &VoteRect.h);
    texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopy(renderer, texture, NULL, &VoteRect);

    sur = TTF_RenderText_Blended(bloodfont_50, skip, red_color);
    TTF_SizeText(bloodfont_50, skip, &SkipRect.w, &SkipRect.h);
    texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopy(renderer, texture, NULL, &SkipRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    for(i = 2; i <= 4; i++){
        SDL_RenderFillRect(renderer, &Srow[i].settingRow);
        sur = TTF_RenderText_Blended(arialfont, Srow[i].text, red_color);
        TTF_SizeText(arialfont, Srow[i].text, &Srow[i].settingRow.w, &Srow[i].settingRow.h);
        texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopy(renderer, texture, NULL, &Srow[i].settingRow);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sur);
    isNight = 1;
}

void GetRoom(SOCKET sockfd, struct sockaddr_in server_addr, PlayerBox *pBox, CurrentPlayer *currUser, char *buffer, char **token){
    int i, j;
    enum pack_type type;
    token = GetToken(buffer, 3);
    type = (enum pack_type)atoi(token[0]);
    if(type == ROOM_INFO){
        playerCount = atoi(token[1]);
        roomMax = atoi(token[2]);
        memset(buffer, 0 , sizeof(*buffer));
        ListenToServer(sockfd, server_addr, buffer);
        token = GetToken(buffer, playerCount * 2 + 1);
        type = (enum pack_type)atoi(token[0]);
        if(type == ROOM_INFO){
            j = 1;
            for(i = 0; i < 12; i++){
                memset(pBox[i].playername, 0, sizeof(*pBox[i].playername));
                pBox[i].player_id = -1;
            }
            for(i = 0; i < playerCount; i++){
                pBox[i].player_id = atoi(token[j++]);
                if(pBox[i].player_id == currUser->id){
                    currBox = &pBox[i];
                    strcpy(pBox[i].playername, "You");
                    j++;
                }else{
                    strcpy(pBox[i].playername, token[j++]);
                }
            }
        }
        cleanToken(token, playerCount * 2 + 1);
    }
}

void GetPlayerList(SDL_Renderer *renderer, PlayerBox *pBox, CurrentPlayer *currUser, SettingRow *Srow){
    int i;
    SDL_FillRect(wSurface, &listBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_Surface *sur;
    TTF_Font * arialfont_10 = TTF_OpenFont("bin/font/arial.ttf", 10);
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 20);
    SDL_Color red_color = {255, 0, 0};
    for(i = 0; i < 12; i++){
        if(pBox[i].player_id != -1){
            sur = IMG_Load(pBox[i].img_link);
            SDL_FillRect(wSurface, &pBox[i].pos, SDL_MapRGB(wSurface->format, 255, 255, 255));
            SDL_BlitSurface(sur, NULL, wSurface, &pBox[i].playerImG);
            rendertext(renderer, arialfont_10, pBox[i].playername, red_color, &pBox[i].nameRect);
            rendertext(renderer, arialfont_10, pBox[i].playerRole, red_color, &pBox[i].roleRect);
        }
    }
    SDL_FillRect(wSurface, &settingBox, SDL_MapRGB(wSurface->format, 0, 0, 0));

    for(i = 0; i < 5; i++){
        memset(Srow[i].text, 0 ,sizeof(*Srow[i].text));
    }
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
    rendertext(renderer, bloodfont_25, "Waiting for other players!", red_color, &NotiText);
    if(currUser->isHost == 1){
        ButtonStart.x = 150;
        ButtonStart.y = 215;
        rendertext(renderer, bloodfont, "Start", red_color, &ButtonStart);
    }
}

void *handleMess(void *argument){
    pthread_detach(pthread_self());
    Arg *arg = (Arg *)argument;
    pthread_t tid;
    int i;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    TTF_Font * bloodfont_40 = TTF_OpenFont("bin/font/font.ttf", 40);
    TTF_Font * bloodfont_25 = TTF_OpenFont("bin/font/font.ttf", 40);
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 20);
    TTF_Font * arialfont_10 = TTF_OpenFont("bin/font/arial.ttf", 10);
    SDL_Color red_color = {255, 0, 0};
    SDL_Color yellow_color = {255, 255, 0};
    printf("[+]Server: %s!\n", arg->buffer);
    while(1){ 
        memset(arg->buffer, 0, sizeof(*(arg->buffer)));
        ListenToServer(arg->sockfd, arg->server_addr, arg->buffer);
        arg->type = GetType(arg->buffer);
        if(arg->type == ROOM_INFO){
            isPlayerJoinning = 1;
            GetRoom(arg->sockfd, arg->server_addr, arg->pBox, arg->currUser, arg->buffer, arg->token);
            GetPlayerList(arg->renderer, arg->pBox, arg->currUser, arg->Srow);
        }else if(arg->type == PUBLIC_MESSAGE_PACK){
            updateChatBox = 1;
            arg->token = GetToken(arg->buffer, 2);
            saveChat(arg->currUser, arg->token[1]);
            if(curr_line == maxLine){
                GetChatBox(arg->renderer, maxLine);
            }
            cleanToken(arg->token, 2);
        }else if(arg->type == START_GAME){
            arg->token = GetToken(arg->buffer, 2);
            arg->currUser->role = atoi(arg->token[1]);
            printf("[+]Your role = %d\n", (int)arg->currUser->role);
            cleanToken(arg->token, 2);
            isStart = 1;
        }
        else if(arg->type == HOST_GAME){
            cleanToken(arg->token, 1);
            arg->currUser->isHost = 1;
            GetButton(arg->renderer, arg->currUser);
        }else if(arg->type == PLAYER_TURN){
            arg->token = GetToken(arg->buffer, 2);
            if(strcmp(arg->token[1], "Night") == 0){
                cleanToken(arg->token , 1);
                isYourTurn = 1;
                printf("[+]Your turn\n");
                SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(arg->renderer, &NotiRect);
                surface = TTF_RenderText_Blended(bloodfont_40, "Your Turn!", yellow_color);
                TTF_SizeText(bloodfont_40, "Your turn!", &NotiText.w, &NotiText.h);
                texture = SDL_CreateTextureFromSurface(arg->renderer, surface);    
                SDL_RenderCopy(arg->renderer, texture, NULL, &NotiText);
                strcpy(arg->Srow[2].text, "Time: Night");
                SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(arg->renderer, &arg->Srow[2].settingRow);
                surface = TTF_RenderText_Blended(arialfont, arg->Srow[2].text, red_color);
                TTF_SizeText(arialfont, arg->Srow[2].text, &arg->Srow[2].settingRow.w, &arg->Srow[2].settingRow.h);
                texture = SDL_CreateTextureFromSurface(arg->renderer, surface);
                SDL_RenderCopy(arg->renderer, texture, NULL, &arg->Srow[2].settingRow);
                SDL_RenderPresent(arg->renderer);
                arg->timeout = 30;
                pthread_create(&tid, NULL, time_countdown, (void *)arg);
            }else if(strcmp(arg->token[1], "NextDay") == 0){
                isNight = -1;
                strcpy(arg->Srow[2].text, "Time: Day");
                SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(arg->renderer, &arg->Srow[2].settingRow);
                surface = TTF_RenderText_Blended(arialfont, arg->Srow[2].text, red_color);
                TTF_SizeText(arialfont, arg->Srow[2].text, &arg->Srow[2].settingRow.w, &arg->Srow[2].settingRow.h);
                texture = SDL_CreateTextureFromSurface(arg->renderer, surface);
                SDL_RenderCopy(arg->renderer, texture, NULL, &arg->Srow[2].settingRow);
                arg->timeout = 30;
                if(isDead == 0){
                    printf("In3");
                    isYourTurn = 1;
                    SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
                    SDL_RenderFillRect(arg->renderer, &NotiRect);
                    surface = TTF_RenderText_Blended(bloodfont_25, "Discussion time!", red_color);
                    TTF_SizeText(bloodfont_25, "Discussion time!", &NotiText.w, &NotiText.h);
                    texture = SDL_CreateTextureFromSurface(arg->renderer, surface);    
                    SDL_RenderCopy(arg->renderer, texture, NULL, &NotiText);
                    arg->timeout = 90;
                    pthread_create(&tid, NULL, time_countdown, (void *)arg);
                }
                SDL_RenderPresent(arg->renderer);
            }
        }else if(arg->type == PLAYER_DEAD){
            arg->token = GetToken(arg->buffer, 2);
            for(i = 0; i < 12; i++){
                if(arg->pBox[i].player_id == atoi(arg->token[1])){
                    SDL_SetRenderDrawColor(arg->renderer, 0, 255, 255, 0);
                    SDL_RenderFillRect(arg->renderer, &arg->pBox[i].pos);
                    SDL_RenderPresent(arg->renderer);
                    arg->pBox[i].isDead = 1;
                    arg->timeout = 0;
                    break;
                }
                if(atoi(arg->token[1]) == arg->currUser->id){
                    isDead = 1;
                    arg->currUser->role = 7;
                    strcpy(arg->Srow[4].text, "Your status: Dead");
                    SDL_SetRenderDrawColor(arg->renderer, 0, 0, 0, 0);
                    SDL_RenderFillRect(arg->renderer, &arg->Srow[4].settingRow);
                    surface = TTF_RenderText_Blended(arialfont, arg->Srow[4].text, red_color);
                    TTF_SizeText(arialfont, arg->Srow[4].text, &arg->Srow[4].settingRow.w, &arg->Srow[4].settingRow.h);
                    texture = SDL_CreateTextureFromSurface(arg->renderer, surface);
                    SDL_RenderCopy(arg->renderer, texture, NULL, &arg->Srow[4].settingRow);
                }
            }
            cleanToken(arg->token, 2);
        }else if(arg->type == ROLE){
            printf("In\n");
            arg->token = GetToken(arg->buffer, 2);
            for(i = 0; i < 12; i++){
                if(arg->pBox[i].player_id == atoi(arg->token[1])){
                    memset(arg->pBox[i].playerRole, 0, sizeof(*arg->pBox[i].playerRole));
                    strcpy(arg->pBox[i].playerRole, "WereWolf");
                    SDL_SetRenderDrawColor(arg->renderer, 255, 255, 255, 0);
                    SDL_RenderFillRect(arg->renderer, &arg->pBox[i].roleRect);
                    surface = TTF_RenderText_Blended(arialfont_10, arg->pBox[i].playerRole, red_color);
                    TTF_SizeText(arialfont_10, arg->pBox[i].playerRole, &arg->pBox[i].roleRect.w, &arg->pBox[i].roleRect.h);
                    texture = SDL_CreateTextureFromSurface(arg->renderer, surface);
                    SDL_RenderCopy(arg->renderer, texture, NULL, &arg->pBox[i].roleRect);
                }
            }
            SDL_RenderPresent(arg->renderer);
            cleanToken(arg->token, 2);
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
    for(i = 0; i < 50; i++){
        groupChat[i] = calloc(100, sizeof(char));
    }
    char *num = calloc(4, sizeof(char));
    Userchat = calloc(MAX_MESSAGE, sizeof(char));
    chatTextBox = calloc(15, sizeof(SDL_Rect));
    for(i = 0; i < 15; i++){
        chatTextBox[i].x = 15;
        chatTextBox[i].y = 265 + 18 * i;
    }
    char **token = makeCleanToken();
    char *SendBuffer = calloc(MAX_MESSAGE ,sizeof(char));
    char *ListenBuffer = calloc(MAX_MESSAGE ,sizeof(char));
    PlayerBox *pBox = calloc(12, sizeof(PlayerBox));
    SettingRow *Srow = calloc(5, sizeof(SettingRow));
    SDL_Surface * surface = NULL;
    SDL_Texture * texture = NULL;
    SDL_Color red_color = {255, 0, 0};
    SDL_Color yellow_color = {255, 255, 0};
    TTF_Font * bloodfont = TTF_OpenFont("bin/font/font.ttf", 40);
    TTF_Font * bloodfont_50 = TTF_OpenFont("bin/font/font.ttf", 50);
    TTF_Font * arialfont = TTF_OpenFont("bin/font/arial.ttf", 10);
    for(i = 0; i < 5; i++){
        Srow[i].text = calloc(MAX_MESSAGE, sizeof(char));
    }
    j = 0;
    for(i = 0; i < 12; i++){
        pBox[i].playerRole = calloc(MAX_NAME, sizeof(char));
        pBox[i].playername = calloc(MAX_NAME, sizeof(char));
        strcpy(pBox[i].playerRole, "???");
        pBox[i].player_id = -1;
        pBox[i].isDead = 0;
        pBox[i].img_link = calloc(100, sizeof(char));

        memset(num, 0 ,sizeof(*num));
        strcat(pBox[i].img_link, "bin/img/pos");
        sprintf(num, "%d", i + 1);
        strcat(pBox[i].img_link, num);
        strcat(pBox[i].img_link, ".jpg");

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
    arg->token = token;
    wSurface = IMG_Load("bin/img/ingame.jpg");

    pthread_create(&tid, NULL, handleMess, (void *)arg);

    SDL_FillRect(wSurface, &chatBox, SDL_MapRGB(wSurface->format, 0, 0, 0));
    SDL_FillRect(wSurface, &ChatField, SDL_MapRGB(wSurface->format, 0, 0, 0));

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
        if(isStart == 1){
            UpdateUI(renderer, currUser, Srow, arg);
            isStart = 2;
            SDL_RenderPresent(renderer);
        }
        while(SDL_PollEvent(&gEvent)){
            switch (gEvent.type){
            case SDL_QUIT:
                memset(SendBuffer, 0, sizeof(*SendBuffer));
                SendBuffer = GetMess(token, 0, EXIT_PACK);
                sendToServer(sockfd, server_addr, SendBuffer);

                SDL_FreeSurface(wSurface);
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                window = NULL;
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
                if(isStart > 0 && isYourTurn == 1){
                    if(check_mouse_pos(SkipRect) == 1){
                        surface = TTF_RenderText_Blended(bloodfont_50, "Skip", yellow_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &SkipRect);
                    }else{
                        surface = TTF_RenderText_Blended(bloodfont_50, "Skip", red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &SkipRect);
                    }
                    if(check_mouse_pos(VoteRect) == 1 && isNight == -1){
                        surface = TTF_RenderText_Blended(bloodfont_50, "Vote", yellow_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &VoteRect);
                    }else{
                        surface = TTF_RenderText_Blended(bloodfont_50, "Vote", red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &VoteRect);
                    }
                    if(check_mouse_pos(Skill_1Rect) == 1 && isNight == 1){
                        surface = TTF_RenderText_Blended(bloodfont_50, skill_1, yellow_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
                    }else{
                        surface = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
                    }
                    if(strlen(skill_2) > 0){
                        if(check_mouse_pos(Skill_2Rect) == 1 && isNight == 1){
                            surface = TTF_RenderText_Blended(bloodfont_50, skill_2, yellow_color);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);    
                            SDL_RenderCopy(renderer, texture, NULL, &Skill_2Rect);
                        }else{
                            surface = TTF_RenderText_Blended(bloodfont_50, skill_2, red_color);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);    
                            SDL_RenderCopy(renderer, texture, NULL, &Skill_2Rect);
                        }
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(check_mouse_pos(ButtonStart) == 1 && isStart == -1){
                    memset(SendBuffer, 0, sizeof(*SendBuffer));
                    SendBuffer = GetMess(token , 0, START_GAME);
                    sendToServer(sockfd, server_addr, SendBuffer);           
                }
                if(check_mouse_pos(ButtonBack) == 1  && isDead == 1){
                    free(groupChat);
                    free(Userchat);
                    SDL_FreeSurface(wSurface);
                    free(chatTextBox);
                    free(skill_1);
                    free(skill_2);
                    free(token);
                    free(SendBuffer);
                    free(ListenBuffer);
                    free(pBox);
                    free(Srow);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                    free(arg);

                    currUser->isHost = -1;
                    currUser->role = -1;
                    memset(currUser->room, 0, sizeof(*currUser->room));
                    currUser->id = -1;
                    return;
                }
                if(check_mouse_pos(SkipRect) == 1 && isStart > 0 && isYourTurn == 1 && isDead == 0){
                    strcpy(token[0], "-1");
                    strcpy(token[1], "0");
                    surface = TTF_RenderText_Blended(bloodfont_50, "Skip", red_color);
                    texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    SDL_RenderCopy(renderer, texture, NULL, &SkipRect);
                    arg->timeout = 0;
                    isYourTurn = -1;
                }
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                for(i = 0; i < 12; i++){
                    if(check_mouse_pos(pBox[i].pos) == 1 && pBox[i].player_id != -1 && pBox[i].isDead == 0){
                        if(targetBox == pBox[i].player_id){
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            targetBox = -1;
                            lastBox = -1;
                        }else if(targetBox != -1){
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &pBox[lastBox].pos);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playername, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playername, &pBox[lastBox].nameRect.w, &pBox[lastBox].nameRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].nameRect);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playerRole, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playerRole, &pBox[lastBox].roleRect.w, &pBox[lastBox].roleRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].roleRect);

                            surface = IMG_Load(pBox[lastBox].img_link);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].playerImG);
                            targetBox = pBox[i].player_id;
                            lastBox = i;
                            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                        }else{
                            targetBox = pBox[i].player_id;
                            lastBox = i;
                            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                        }
                        SDL_RenderFillRect(renderer, &pBox[i].pos);

                        surface = TTF_RenderText_Blended(arialfont, pBox[i].playername, red_color);
                        TTF_SizeText(arialfont, pBox[i].playername, &pBox[i].nameRect.w, &pBox[i].nameRect.h);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_RenderCopy(renderer, texture, NULL, &pBox[i].nameRect);

                        surface = TTF_RenderText_Blended(arialfont, pBox[i].playerRole, red_color);
                        TTF_SizeText(arialfont, pBox[i].playerRole, &pBox[i].roleRect.w, &pBox[i].roleRect.h);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_RenderCopy(renderer, texture, NULL, &pBox[i].roleRect);

                        surface = IMG_Load(pBox[i].img_link);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_RenderCopy(renderer, texture, NULL, &pBox[i].playerImG);
                    }
                }
                if(isYourTurn == 1 && isStart > 0 && isNight == 1){
                    if(check_mouse_pos(Skill_1Rect) == 1){
                        sprintf(arg->token[0], "%d", targetBox);
                        strcpy(arg->token[1], "1");
                        surface = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
                        arg->timeout = 0;
                        isYourTurn = -1;
                        targetBox = -1;
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &pBox[lastBox].pos);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playername, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playername, &pBox[lastBox].nameRect.w, &pBox[lastBox].nameRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].nameRect);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playerRole, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playerRole, &pBox[lastBox].roleRect.w, &pBox[lastBox].roleRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].roleRect);

                            surface = IMG_Load(pBox[lastBox].img_link);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].playerImG);
                        lastBox = -1;
                    }else if(check_mouse_pos(Skill_2Rect) == 1 && currUser->role == WITCH){
                        sprintf(arg->token[0], "%d", targetBox);
                        strcpy(arg->token[1], "-1");
                        surface = TTF_RenderText_Blended(bloodfont_50, skill_2, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &Skill_2Rect);
                        arg->timeout = 0;
                        isYourTurn = -1;
                        targetBox = -1;
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &pBox[lastBox].pos);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playername, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playername, &pBox[lastBox].nameRect.w, &pBox[lastBox].nameRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].nameRect);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playerRole, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playerRole, &pBox[lastBox].roleRect.w, &pBox[lastBox].roleRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].roleRect);

                            surface = IMG_Load(pBox[lastBox].img_link);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].playerImG);
                        lastBox = -1;
                    }
                    
                    // else if(check_mouse_pos(VoteRect) == 1 && isNight == -1){
                    //     sprintf(token[0], "%d", targetBox);
                    //     strcpy(token[1], "0");
                    //     surface = TTF_RenderText_Blended(bloodfont_50, "Vote", red_color);
                    //     texture = SDL_CreateTextureFromSurface(renderer, surface);    
                    //     SDL_RenderCopy(renderer, texture, NULL, &VoteRect);
                    //     arg->timeout = 0;
                    //     isYourTurn = -1;
                    // }
                }
                if(isYourTurn == 1 && isStart > 0 && isNight != 1){
                    if(check_mouse_pos(VoteRect) == 1){
                        sprintf(arg->token[0], "%d", targetBox);
                        strcpy(arg->token[1], "1");
                        surface = TTF_RenderText_Blended(bloodfont_50, skill_1, red_color);
                        texture = SDL_CreateTextureFromSurface(renderer, surface);    
                        SDL_RenderCopy(renderer, texture, NULL, &Skill_1Rect);
                        arg->timeout = 0;
                        isYourTurn = -1;
                        targetBox = -1;
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                            SDL_RenderFillRect(renderer, &pBox[lastBox].pos);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playername, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playername, &pBox[lastBox].nameRect.w, &pBox[lastBox].nameRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].nameRect);

                            surface = TTF_RenderText_Blended(arialfont, pBox[lastBox].playerRole, red_color);
                            TTF_SizeText(arialfont, pBox[lastBox].playerRole, &pBox[lastBox].roleRect.w, &pBox[lastBox].roleRect.h);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].roleRect);

                            surface = IMG_Load(pBox[lastBox].img_link);
                            texture = SDL_CreateTextureFromSurface(renderer, surface);
                            SDL_RenderCopy(renderer, texture, NULL, &pBox[lastBox].playerImG);
                        lastBox = -1;
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            case SDL_MOUSEWHEEL:
                if(check_mouse_pos(chatBox) == 1){
                    if(gEvent.wheel.y > 0 && curr_line > 15){
                        curr_line--;
                        GetChatBox(renderer, curr_line);
                    }else if(gEvent.wheel.y < 0 && curr_line < maxLine){
                        curr_line++;
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
                        cleanToken(arg->token, 1);
                        sendToServer(sockfd, server_addr, SendBuffer);
                        
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderFillRect(renderer, &ChatField);
                        memset(SendBuffer, 0, sizeof(*SendBuffer));
                        strcat(SendBuffer, "You: ");
                        strcat(SendBuffer, Userchat);

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




