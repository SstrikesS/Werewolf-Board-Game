all : server client
server:
	gcc server.c -lws2_32 -o server
client:
	gcc client.c -lws2_32 -o client
clean:
	del server.exe client.exe