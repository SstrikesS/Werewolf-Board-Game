#include "packet.h"
#include <unistd.h>

enum pack_type GetMessageType(char *message){
    char *tmp = calloc(4, sizeof(char));
    tmp = strtok(message, "|");
    enum pack_type type = (enum pack_type)atoi(tmp);
    return type;
}

char **GetToken(char *message, int token_size){
	int i;
    char **token = calloc(token_size, sizeof(char *));
    char *tmp = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(tmp, '\0', MAX_OF_CHAR_MESSAGE);
    for(i = 0; i < 3; i++){
        token[i] = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
        memset(token, '\0', MAX_OF_CHAR_MESSAGE);
    }
    tmp = strtok(message, "|");
    strcpy(token[0], tmp);
    i = 1;
    while(1){
        tmp = strtok(NULL, "|");
        if(tmp != NULL){
            strcpy(token[i], tmp);
            i++;
        }else{
            break;
        }
    }
    return token;
}

char *GetMessage_Login(char *client_name){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    enum pack_type type = LOGIN_PACK;
    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, client_name);
    return packet_str;
}

char *GetMessage_PubMess(char *message, char *client_name){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    enum pack_type type = PUBLIC_MESSAGE_PACK;
    sprintf(type_str, "%d", (int)type);
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, client_name);
    strcat(packet_str, "|");
    strcat(packet_str, message);
    return packet_str;
}

char *GetMessage_SUCCESS(char *message){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    enum pack_type type = SUCCEED_PACK;
    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, message);
    return packet_str;
}

char *GetMessage_ERROR(char *message){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    enum pack_type type = ERROR_PACK;
    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    strcat(packet_str, "|");
    strcat(packet_str, message);
    return packet_str;
}

char *RoomPacktoString(enum pack_type type, char *roomName, char *max_player){
    char *pack_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(pack_str, '\0', sizeof(char));
    char *type_str = calloc(1, sizeof(char));
    memset(type_str, '\0', sizeof(char));
    sprintf(type_str, "%d", (int)type);
    strcat(pack_str, type_str);
    strcat(pack_str, "|");
    strcat(pack_str, roomName);
    strcat(pack_str, "|");
    strcat(pack_str, max_player);
    return pack_str;
}
