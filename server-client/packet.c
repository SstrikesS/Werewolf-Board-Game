#include "packet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *PacketParse(Packet *pack){
    char type_str[1];
    char *packet_str = (char *)malloc(sizeof(char) * MAX_OF_CHAR_MESSAGE);
    sprintf(type_str, "%d", (int)pack->type);   
    strcat(packet_str, type_str);
    strcat(packet_str, ";");
    strcat(packet_str, pack->message);
    return packet_str;
}
Packet *MessageParse(char *message){
	int i;
    Packet *pack = (Packet *)malloc(sizeof(Packet));
    char c = message[0];
    int type = (int)c - 48;
    pack->type = (enum pack_type)type;
    for(i = 2; i < strlen(message); i++){
        pack->message[i - 2] = message[i];
    }
    pack->message[strlen(message) - 2] = '\0';
    return pack;
}