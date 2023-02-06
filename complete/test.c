#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
typedef struct _A{
    int a;
}A;
char **GetToken(char *message, int tokenSize){
    int i;
    char **token = calloc(tokenSize, sizeof(char *));
    char *tmp = calloc(1000, sizeof(char));
    memset(tmp, 0, sizeof(*tmp));
    for(i = 0; i < tokenSize; i++){
        token[i] = calloc(1000, sizeof(char));
        memset(token[i], 0, sizeof(*token[i]));
    }
    tmp = strtok(message, "|");
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
    free(tmp);
    return token;
}

char *GetMess(char **token, int tokenSize, enum pack_type type){
    char *type_str = calloc(4, sizeof(char));
    char *packet_str = calloc(1000, sizeof(char));
    
    memset(packet_str, 0, sizeof(*packet_str));

    sprintf(type_str, "%d", (int)type);   
    strcat(packet_str, type_str);
    int i;
    for(i = 0; i < tokenSize; i++){
        strcat(packet_str, "|");
        strcat(packet_str, token[i]);
    }
    free(type_str);
    return packet_str;
}

int main(){
    // char *message = calloc(100, sizeof(char)); 
    // strcpy(message, "5");
    // char **token = calloc(10, sizeof(char *));
    // int i;
    // for(i = 0; i < 10; i++){
    //     token[i] = calloc(1000, sizeof(char));
    //     memset(token[i], '\0', sizeof(*token[i]));
    // }
    // // char *mess2 = calloc(1000, sizeof(char));
    // // memset(mess2, 0, sizeof(*mess2));
    // // strcpy(mess2, message);
    // // printf("%s\n", mess2);
    // // strcpy(token[0], "thanhnt");
    // // strcpy(token[1], "hello");
    // // message = GetMess(token, 2, LOGIN_PACK);
    // printf("%s\n", message);
    // for(i = 0; i < 10; i++){
    //     memset(token[i], 0, sizeof(*token[i]));
    // }
    // token = GetToken(message, 5);
    // for(i = 0; i < 5; i++){
    //     printf("%s\n", token[i]);
    // }
    // for(i = 0; i < 5; i++){
    //     free(token[i]);
    // }
    // free(token);
    int i;
    char **tmp = calloc(1, sizeof(char *));
    for(i = 0; i < 2; i++){
        tmp[i] = calloc(100, sizeof(char));
        strcpy(tmp[i], "hello");
    }
    for(i = 0; i < 2; i++){
        memset(tmp[i], 0, sizeof(*tmp[i]));
    }
    printf("%s\n", tmp[1]);
    // A *tmp = calloc(1, sizeof(A));
    // tmp->a = 1;
    // A *tmp2 = tmp;
    // tmp2 = NULL;
    // //memset(tmp2, 0, sizeof(*tmp2));
    // printf("%d\n", tmp->a);
    return 0;
}