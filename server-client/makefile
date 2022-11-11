all : server client
server: linklist.o server.c
	gcc linklist.o server.c -lws2_32 -o server
client: client.c
	gcc client.c -lws2_32 -o client
linklist.o: linklist.h linklist.c
	gcc linklist.h -c linklist.c
clean:
	del server.exe client.exe