all: myServer client

myServer: server.c tranferQueue.c
	gcc -Wall -g server.c tranferQueue.c -o server -lpthread
	
server: server.c
	gcc -Wall -g server.c -o server -lpthread

client: client.c
	gcc -Wall -g client.c -o client

clean:
	rm server client