#include "definelink.h"
enum pack_type{
    EXIT_PACK = 0, // thoat game
    PUBLIC_MESSAGE_PACK = 1, // mess global
    PRIVATE_MESSAGE_PACK = 2,// mess cua bon soi
    ROOM_PACK = 3, // tao game va setting game
    JOIN_GAME = 4, // tin hieu join game
    VOTE = 5, // packet mang thong tin tat ca loai vote cua nguoi choi(vote treo co , vote skip time, ...)
    PLAYER_TARGET = 6, // chua thong tin phayer target (vd soi taget trungvt, protector bao ve trungvt, seer doan trungvt)
    PLAYER_DEAD = 7 // cac player dead
};

typedef struct Packet{
    char message[MAX_OF_CHAR_MESSAGE];
    enum pack_type type;
}Packet;


char *PacketParse(Packet *pack);
Packet *MessageParse(char *message);

