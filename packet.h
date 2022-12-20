#include "definelink.h"

enum pack_type{
    EXIT_PACK = 0, // thoat game
    LOGIN_PACK = 1, // first time connect to server
    PING_PACK = 2, // pack dung de kiem tra trang thai cua client
    PUBLIC_MESSAGE_PACK = 3, // mess global
    PRIVATE_MESSAGE_PACK = 4,// mess cua bon soi
    ROOM_PACK = 5, // tao game va setting game
    JOIN_GAME = 6, // tin hieu join game
    VOTE = 7, // packet mang thong tin tat ca loai vote cua nguoi choi(vote treo co , vote skip time, ...)
    PLAYER_TARGET = 8, // chua thong tin phayer target (vd soi taget trungvt, protector bao ve trungvt, seer doan trungvt)
    PLAYER_DEAD = 9, // cac player dead
};

typedef struct Packet{
    char message[MAX_OF_CHAR_MESSAGE];
    enum pack_type type;
}Packet;


int GetPackType(char *message);

char *PacketParse(Packet *pack);

Packet *MessageParse(char *message);

char *PacketParse_PubMess(Packet *pack, char *client_name);

Packet *MessageParse_PubMess(char *message, char *client_name);

void PackCopy(Packet *pack1, Packet *pack2);