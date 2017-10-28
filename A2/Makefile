all: client server

server: server.c
	gcc -Wall -g server.c tranferQueue.c -o server -lpthread

client: client.c
	gcc -Wall -g client.c -o client

clean:
	rm server client