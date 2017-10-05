/**
 * server.c
 * Christopher Katsaras
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define MAXBUFLEN 4096
int main(int argc, char *argv[]) {

	char * portNumber;
	int userSocket; 
	struct sockaddr_in serv;
	struct sockaddr_in client;
	char * buffer = malloc(sizeof(MAXBUFLEN + 1)); 
	int len;

	socklen_t socksize = sizeof(struct sockaddr_in);
	if(argc == 2) {
		portNumber = argv[1];
		printf("Listening on port %s\n", portNumber);
	} else {
		perror("Error: Missing port number!\n");
		exit(EXIT_FAILURE);
	}

	memset(&serv, 0, sizeof(serv));

	serv.sin_family = AF_INET;                
	serv.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv.sin_port = htons(atoi(portNumber));           

	userSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(userSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(EXIT_FAILURE);
	}

	listen(userSocket, 10);

	int connection = accept(userSocket, (struct sockaddr *)&client, &socksize);

	while(connection) {
		printf("Incoming connection from %s \n", inet_ntoa(client.sin_addr));
		len = recv(connection, buffer, MAXBUFLEN, 0);

		//While there is more data to get
		while(len > 0) {
			buffer[len] = '\0';
			len = recv(connection, buffer, MAXBUFLEN, 0);
		}
		printf("\n\n\n");
		
		close(connection);
		connection = accept(userSocket, (struct sockaddr *)&client, &socksize);
	}

	close(userSocket);
	return 0;
}