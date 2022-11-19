all : server client
server: linklist.o  packet.o server.c
	gcc linklist.o packet.o server.c -lws2_32 -o server
client: packet.o client.c 
	gcc packet.o client.c -lws2_32 -o client
linklist.o: linklist.h linklist.c
	gcc linklist.h -c linklist.c
packet.o: packet.c packet.h
	gcc packet.h -c packet.c
clean:
	del server.exe client.exe linklist.o packet.o packet.h.gch linklist.h.gch