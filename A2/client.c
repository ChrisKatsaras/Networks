/**
 * client.c
 * Christopher Katsaras
 */
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
#include <sys/time.h>

 
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[]) {
	int MAXBUFLEN = 4096;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	char * buffer = calloc(MAXBUFLEN + 1,sizeof(char));
	char hostName[100] = "localhost";
	char portString[100];
	char ipAddr[INET6_ADDRSTRLEN];
	char * temp;
	int rv;
	int mysocket;
	struct sockaddr_in dest;
	//struct timeval start, stop; Code for timing execution
    
 	if(argc != 2 && argc != 3 && argc != 4) {
 		perror("Wrong number of arguements!");
 		EXIT_FAILURE;
 	} else {
 		temp = strtok(argv[1],":");
 		if(temp != NULL) {
 			strcpy(hostName, temp);
	 		temp = strtok(NULL, ":");
	 		if(temp != NULL) {
	 			strcpy(portString,temp);
	 		} else {
	 			printf("Incorrect format! Correct format is hostname:portnum\n");
	 			exit(EXIT_FAILURE);
	 		}
 		} else {
 			printf("Incorrect format! Correct format is hostname:portnum\n");
 			exit(EXIT_FAILURE);
 		}

 		//If the user specifies how big the buffer is
 		if(argc == 3) {
 			MAXBUFLEN = atoi(argv[2]);
 			if(MAXBUFLEN == 0) {
 				printf("Invalid value for buffer length!\n");
 				exit(EXIT_FAILURE);
 			}
 			free(buffer);
 			buffer = calloc(MAXBUFLEN + 1,sizeof(char)); 
 		}
 	}

 	//Initialize infor for getaddr
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

 	if ((rv = getaddrinfo(hostName, portString, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)servinfo->ai_addr), ipAddr, sizeof ipAddr);
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	memset(&dest, 0, sizeof(dest));
	
	dest.sin_family = AF_INET;
	inet_aton(ipAddr, &(dest.sin_addr));
	dest.sin_port = htons(atoi(portString));               
 	
 	//Connects to server
	if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) != 0) {
		perror("Client unable to connect to server");
		exit(EXIT_FAILURE);
	}

	char ch;
	int count = 0;
	
	//Code for timing execution
	//gettimeofday(&start, NULL);
	strcpy(buffer, argv[2]);
	strcat(buffer, " ");
	if(send(mysocket, buffer , strlen(buffer), 0) != strlen(buffer)) {
		printf("Send failed!\n");
	}
	strcpy(buffer, argv[3]);
	if(send(mysocket, buffer , strlen(buffer), 0) != strlen(buffer)) {
		printf("Send failed!\n");
	}

	while ((ch = fgetc(stdin)) != EOF) {
		if(count > MAXBUFLEN) {
			count = 0;
			if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
				printf("Send failed!\n");
			}
			free(buffer);
			buffer = calloc(MAXBUFLEN + 1,sizeof(char)); 
		} else {
			buffer[count] = ch;
			count++;
		}
	}

	if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
		printf("Send failed!\n");
	}
	
	/*
	Code for timing execution
	gettimeofday(&stop, NULL);

	double result = (((stop.tv_sec - start.tv_sec) + ((double)stop.tv_usec) / 1000000) - ((double)start.tv_usec) / 1000000);

	printf("%lf\n", result);*/
 
	close(mysocket);
	return EXIT_SUCCESS;
}
