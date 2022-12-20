#include "packet.h"
#include <unistd.h>

int GetPackType(char *message){
    char c = message[0];
    int type = (int)c - 48;
    return type;
}

char *PacketParse(Packet *pack){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    sprintf(type_str, "%d", (int)pack->type);   
    strcat(packet_str, type_str);
    strcat(packet_str, ";");
    strcat(packet_str, pack->message);
    return packet_str;
}

Packet *MessageParse(char *message){
	int i;
    Packet *pack = calloc(1, sizeof(Packet));
    int type = GetPackType(message);
    pack->type = (enum pack_type)type;
    for(i = 2; i < strlen(message); i++){
        pack->message[i - 2] = message[i];
    }
    pack->message[strlen(message) - 2] = '\0';
    return pack;
}

char *PacketParse_PubMess(Packet *pack, char *client_name){
    char *type_str = calloc(1, sizeof(char));
    char *packet_str = calloc(MAX_OF_CHAR_MESSAGE, sizeof(char));
    memset(packet_str, '\0', MAX_OF_CHAR_MESSAGE);
    sprintf(type_str, "%d", (int)pack->type);
    strcat(packet_str, type_str);
    strcat(packet_str, ";");
    strcat(packet_str, client_name);
    strcat(packet_str, ";");
    strcat(packet_str, pack->message);
    return packet_str;
}

Packet *MessageParse_PubMess(char *message, char *client_name){
	int i, j;
    Packet *pack = calloc(1, sizeof(Packet));
    int type = GetPackType(message);
    pack->type = (enum pack_type)type;
    for(i = 2; i < strlen(message); i++){
        while(message[i] == ';'){
            j = ++i;
            break;
        }
        client_name[i - 2] = message[i];
    }
    client_name[j - 2] = '\0';
    for(i = j; i < strlen(message); i++){
        pack->message[i - j]= message[i];
    }
    pack->message[strlen(message) - j] = '\0';
    return pack;
}
