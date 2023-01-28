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
    SUCCEED_PACK = 10, // tra lai thong bao thanh cong
    ERROR_PACK = 11// tra lai thong bao loi
};

typedef struct Packet{
    char message[MAX_OF_CHAR_MESSAGE];
    enum pack_type type;
}Packet;


enum pack_type GetMessageType(char *message);

char **GetToken(char *message, int token_size);

char *GetMessage_Login(char *client_name);

char *GetMessage_PubMess(char *message, char *client_name);

char *GetMessage_SUCCESS(char *message);

char *GetMessage_ERROR(char *message);



char *RoomPacktoString(enum pack_type type, char *roomName, char *max_player);