all : server client
server: linklist.o packet.o player.o server.c
	gcc linklist.o packet.o player.o server.c -pthread -lws2_32 -o server
client: packet.o client.c 
	gcc packet.o client.c -pthread -lws2_32 -o client
linklist.o: linklist.h linklist.c
	gcc linklist.h -c linklist.c
packet.o: packet.c packet.h
	gcc packet.h -c packet.c
player.o: player.c player.h
	gcc player.h -c player.c
clean:
	del server.exe client.exe linklist.o packet.o player.o packet.h.gch linklist.h.gch player.h.gch
