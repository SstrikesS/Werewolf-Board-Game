all :server WerewolfBoardGame
WerewolfBoardGame: WerewolfBoardGame.c MenuGame.o HostGame.o JoinGame.o WaitingRoom.o client.o pack.o
	gcc MenuGame.o HostGame.o JoinGame.o WaitingRoom.o client.o pack.o WerewolfBoardGame.c -I src\include -L src\lib -Wall -pthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image -o WerewolfBoardGame
MenuGame.o: MenuGame.h MenuGame.c
	gcc MenuGame.h -I src\include -L src\lib -Wall -pthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image -c MenuGame.c 
HostGame.o: HostGame.h HostGame.c
	gcc HostGame.h -I src\include -L src\lib -Wall -pthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image -c HostGame.c
JoinGame.o: JoinGame.h JoinGame.c
	gcc JoinGame.h -I src\include -L src\lib -Wall -pthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image -c JoinGame.c
WaitingRoom.o: WaitingRoom.h WaitingRoom.c
	gcc WaitingRoom.h -I src\include -L src\lib -Wall -pthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -lSDL2_image -c WaitingRoom.c
server: server.c pack.o
	gcc pack.o server.c -Wall -pthread -lws2_32 -o server
client.o: client.h client.c
	gcc client.h -Wall -pthread -lws2_32 -c client.c
pack.o: pack.h pack.c
	gcc pack.h -Wall -c pack.c
clean:
	del  init.o init.h.gch MenuGame.o MenuGame.h.gch HostGame.o HostGame.h.gch JoinGame.o JoinGame.h.gch WaitingRoom.o WaitingRoom.h.gch pack.o pack.h.gch client.o client.h.gch