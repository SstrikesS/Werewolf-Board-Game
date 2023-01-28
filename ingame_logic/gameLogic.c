#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DEAD -1
#define ALIVE 1
#define SKILL_PROTECT
#define SKILL_THROW

enum Role{ // cac Role
    WEREWOLF = 1,
    WITCH = 2,
    PROTECTER = 3,
    SEER = 4,
    HUNTER = 5,
    VILLAGE = 6,
    SPECTATOR = 7 // khi chet se tro thanh role nay
};

typedef struct Player{ // player trong 1 game
    char* player_name; //ten player
    int id; // id cua player
    int heal; //mau cua player
    enum Role role; // role
    int skill;// skill dung cho WITCH
    int vote_count; // so vote cua cac player khac
}Player;

Player *p; //list player trong game

int w = 3; // so luong soi
int v = 9; // so luong dan

void randomRole(){// ham random role
    // Todo:
}

int isDead(Player player){// kiem tra player chet chua
    if(player.heal <= 0){
        return DEAD;
    }else{
        return ALIVE;
    }
}

int findPlayer(int id){//tim kiem player theo id. Tra ve index cua mang p[]
    int i;
    for(i = 0; i < 12; i++){
        if(p[i].id == id){
            return i; 
        }
    }
    return -1; //tra ve -1 neu ko tim dc
}

void vote(Player player1 , Player player2){ // vote trong buoi sang, player1 vote player2
    if(player2.role != SPECTATOR){
        player2.vote_count++;
    }else{
        printf("This player is already dead\n");
    }
} 

int handle_vote(){ // tim ra nguoi duy nhat co so vote cao nhat, tra ve index cua mang p[]
    int i, max = 0;
    for(i = 0; i < 12; i++){
        if(p[i].vote_count > max){
            max = p[i].vote_count;
        }
    }
    // check truong hop nhieu hon 1 nguoi co so luong vote cao nhat
    // Todo:
    return -1; // ko co ai se tra ve -1
}

void target(Player player1 , Player player2){// player1 target player2
    if(player2.role != SPECTATOR){
        switch (player1.role){
        case WEREWOLF:
            player2.heal--; // bi can mat 1 mau
            break;
        case WITCH:
            //  co 2 skill bao ve va nem binh
            //  scanf("%d", player2.skill); hoac them parameter vao ham target(Player player1, Player player2, int skill)
            // Todo:
            break;
        case PROTECTER:
            // Todo:
            break;
        case HUNTER:
            // Todo:
            break;
        case SEER:
            // Todo:
            break;
        default:
            // Todo:
            break;
        }
    }else{
        printf("%s is already dead\n", player2.player_name);
    }
   
}

void Day(){
    // Todo:

    //sau khi hona thanh vote
    int index = handle_vote(); // kiem tra ket qua vote
    if(index != -1){
        p[index].heal = -1; // player bi vote cao nhat heal se ve -1
    }
}

void Night(){
    //ban dem phai goi theo thu tu cac role dinh san (Protecter->WereWolf->Seer->Witch->Hunter)
    // Todo:
}

void CheckGameStatus(){// check game sau 1 ngay/ 1 dem
    int i;
    for(i = 0; i < 12; i++){
        if(isDead(p[i]) == DEAD){
            printf("Player %s is eliminated\n", p[i].player_name);
            if(p[i].role == WEREWOLF){
                w--; //so luong soi giam
            }else{
                v--; //so luong nguoi giam
            }
            p[i].role = SPECTATOR;// chuyen role
        }
    }
    //Note: Sau moi dem phai reset mau(player.heal) ve 1 neu player do con song(ALIVE)
    for(i = 0; i < 12; i++){
        if(p[i].role != SPECTATOR){
            p[i].heal = 1;
        }
    }
}
int main(){
    p = calloc(12, sizeof(Player));
    int i;
    // test case
    for(i = 0; i < 12; i++){            
        p[i].id = i + 1;
        p[i].heal = 1;
        p[i].skill = 0;
        p[i].vote_count = 0;
        if(i < 3){
            p[i].player_name = calloc(17, sizeof(char));
            char num[2];
            strcat(p[i].player_name, "WereWolf ");
            sprintf(num, "%d", i + 1);
            strcat(p[i].player_name, num);
            p[i].player_name[strlen(p[i].player_name)] = '\0';
            p[i].role = WEREWOLF;
        }else if(i == 3){
            p[i].player_name = calloc(17, sizeof(char));
            strcpy(p[i].player_name, "Witch");
            p[i].role = WITCH;
        }else if(i == 4){
            p[i].player_name = calloc(17, sizeof(char));
            strcpy(p[i].player_name, "Protecter");
            p[i].role = PROTECTER;
        }
        else if(i == 5){
            p[i].player_name = calloc(17, sizeof(char));
            strcpy(p[i].player_name, "Seer");
            p[i].role = SEER;
        }
        else if(i == 6){
            p[i].player_name = calloc(17, sizeof(char));
            strcpy(p[i].player_name, "Hunter");
            p[i].role = HUNTER;
        }
        else{
            p[i].player_name = calloc(17, sizeof(char));
            char num[2];
            strcat(p[i].player_name, "Village ");
            sprintf(num, "%d", i + 1);
            strcat(p[i].player_name, num);
            p[i].player_name[strlen(p[i].player_name)] = '\0';
            p[i].role = VILLAGE;
        }
    }
    //vong lap den het game
    while(w >= v || w == 0){
    //ban ngay
        Day();
        CheckGameStatus();
    //ban dem
        Night();
        CheckGameStatus();
    }

    return 0;
}
