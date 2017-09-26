#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXRCVLEN 500
 
int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *servinfo;
	char buffer[MAXRCVLEN + 1]; 
	char * msg = "Chris message\n";
	char * ip = NULL;
	char * temp;
	char portString[100];
	int port = 0;
	int rv;
	int len;
	int mysocket;
	struct sockaddr_in dest;

 	
 	if(argc != 2) {
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
 	}
 	  //Initialize infor for getaddr
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

 	if ((rv = getaddrinfo(ip, portString, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	memset(&dest, 0, sizeof(dest));
	
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
	dest.sin_port = htons(port);               
 	
	connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

	send(mysocket, msg, strlen(msg), 0); 
	len = recv(mysocket, buffer, MAXRCVLEN, 0);
 	
	buffer[len] = '\0';
 
	printf("Received %s (%d bytes).\n", buffer, len);
 
	close(mysocket);
	return EXIT_SUCCESS;
}
