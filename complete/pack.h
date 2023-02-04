#include "globaldefine.h"
enum pack_type{
    EXIT_PACK = 0, // thoat game
    LOGIN_PACK = 1, // first time connect to server
    JOIN_ROOM = 2, // pack dung de kiem tra trang thai cua client
    PUBLIC_MESSAGE_PACK = 3, // mess global
    PRIVATE_MESSAGE_PACK = 4,// mess cua bon soi
    HOST_GAME = 5, // tao game va setting game hoac list room
    JOIN_GAME = 6, // tin hieu join game
    VOTE = 7, // packet mang thong tin tat ca loai vote cua nguoi choi(vote treo co , vote skip time, ...)
    PLAYER_TARGET = 8, // chua thong tin phayer target (vd soi taget trungvt, protector bao ve trungvt, seer doan trungvt)
    PLAYER_DEAD = 9, // cac player dead
    SUCCEED_PACK = 10, // tra lai thong bao thanh cong
    ERROR_PACK = 11,// tra lai thong bao loi
    ROOM_INFO = 12,
    START_GAME = 13,
    PLAYER_TURN = 14
};

typedef struct _CurrentPlayer{
    char *name;
    int id;
    char *room;
    int isHost;
    int role;
}CurrentPlayer;

enum pack_type GetType(char *message);

char *GetMess(char **token, int tokenSize, enum pack_type type);

char **GetToken(char *message, int tokenSize);

char **makeCleanToken();

