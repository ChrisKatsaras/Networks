#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
 
int main(int argc, char *argv[])
{
	int MAXBUFLEN = 10;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	//FILE *fp = fopen(stdin, "r");
	char * buffer = malloc(sizeof(MAXBUFLEN + 1)); 
	char * msg = "Sending a message over! Kind of big, so we gotta break it!";
	char * ip = NULL;
	char * temp;
	bool breakingUp = false;
	char portString[100];
	int port = 0;
	int rv;
	int i;
	int counter = 0;
	//int len;
	int mysocket;
	struct sockaddr_in dest;

 	
 	if(argc != 2 && argc != 3) {
 		perror("Wrong number of arguements!");
 		EXIT_FAILURE;
 	} else {
 		temp = strtok(argv[1],":");
 		if(temp != NULL) {
 			ip = temp;
	 		temp = strtok(NULL, ":");
	 		port = atoi(temp);
	 		sprintf(portString, "%d", port);
	 		//printf("%s\n%d\n", ip, port);
 		}

 		//If the user specifies how big the buffer is
 		if(argc == 3) {
 			buffer = malloc(sizeof(atoi(argv[2]) + 1)); 
 		}
 	}
 	//Initialize infor for getaddr
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

 	if ((rv = getaddrinfo(ip, portString, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	memset(&dest, 0, sizeof(dest));
	
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(ip);
	//dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	dest.sin_port = htons(port);               
 	
 	//Connects to server
	if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) != 0) {
		perror("Client unable to connect to server");
		exit(EXIT_FAILURE);
	}

	//Needs to break the message into smaller chunks
	if(strlen(msg) > MAXBUFLEN) {
		int j;
		printf("Need to break up\n");
		breakingUp = true;
		while(breakingUp) {
			j=0;
			if(strlen(msg) - counter > MAXBUFLEN) {
				
				for(i=counter;i<counter+MAXBUFLEN;i++) {
					//printf("%c", msg[i]);
					buffer[j] = msg[i];
					j++;
				}
				buffer[MAXBUFLEN] = '\0';
				printf("%s  %lu\n",buffer, strlen(buffer));
				sleep(1);//TEST
				if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
					printf("Send failed!\n");
				}
				counter += MAXBUFLEN;
			} else {

				buffer = malloc(sizeof((strlen(msg) - counter)+1)); 
				for(i=counter;i<strlen(msg);i++) {
					buffer[j] = msg[i];
					j++;
				}
				printf("%s  %lu\n",buffer, strlen(buffer));
				sleep(1);//TEST
				if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
					printf("Send failed!\n");
				}
				counter += (strlen(msg) - counter);
				breakingUp = false;
			}
		}
		
	} else {
		send(mysocket, msg, strlen(msg), 0); 
	}

	/*len = recv(mysocket, buffer, MAXBUFLEN, 0);
 	
	buffer[len] = '\0';
 
	printf("Received %s (%d bytes).\n", buffer, len);*/
 
	close(mysocket);
	return EXIT_SUCCESS;
}
