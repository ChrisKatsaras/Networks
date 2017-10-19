all: server client
	
server: server.c
	gcc -Wall -g server.c -o server

client: client.c
	gcc -Wall -g client.c -o client

clean:
	rm server client