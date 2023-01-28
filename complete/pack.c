#include "pack.h"

enum pack_type GetType(char *message){
    char *tmp = calloc(MAX_MESSAGE, sizeof(char));
    strcpy(tmp, message);
    strtok(tmp, "|");
    enum pack_type type = (enum pack_type)atoi(tmp);
    free(tmp);
    return type;
}

char **GetToken(char *message, int tokenSize){
    int i;
    char **token = calloc(tokenSize, sizeof(char *));
    char *tmp = calloc(MAX_MESSAGE, sizeof(char));
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    strcpy(buffer, message);
    for(i = 0; i < tokenSize; i++){
        token[i] = calloc(MAX_MESSAGE, sizeof(char));
    }
    tmp = strtok(buffer, "|");
    strcpy(token[0], tmp);
    i = 1;
    while(i < tokenSize){
        tmp = strtok(NULL, "|");
        if(tmp != NULL){
            strcpy(token[i], tmp);
            i++;
        }else{
            break;
        }
    }
    free(buffer);
    free(tmp);
    return token;
}

char *GetMess(char **token, int tokenSize, enum pack_type type){
    char *type_str = calloc(4, sizeof(char));
    char *packet_str = calloc(MAX_MESSAGE, sizeof(char));
    
    memset(packet_str, 0, sizeof(*packet_str));

    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    int i;
    for(i = 0; i < tokenSize; i++){
        strcat(packet_str, "|");
        strcat(packet_str, token[i]);
    }
    return packet_str;
}


char *GetLoginMess(char *name){
    char *type_str = calloc(4, sizeof(char));
    char *packet_str = calloc(MAX_MESSAGE, sizeof(char));
    
    memset(packet_str, 0, sizeof(*packet_str));
    enum pack_type type = LOGIN_PACK;
    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, name);
    return packet_str;
}

char *GetNotiMess(char *noti, enum pack_type type){
    char *type_str = calloc(4, sizeof(char));
    char *packet_str = calloc(MAX_MESSAGE, sizeof(char));

    memset(packet_str, 0, sizeof(*packet_str));
    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, noti);
    return packet_str;
}
