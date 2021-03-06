/**
 * client.c
 * Christopher Katsaras & Connor Geddes
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
#include <stdint.h>
 
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
	char ch;
	int count = 0;
	struct sockaddr_in dest;
    
 	if(argc != 2 && argc != 3 && argc != 4 && argc != 5 && argc != 6) {
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

	//Sends chunk size
	uint64_t t1 = atoi(argv[2]);
	uint64_t chunkSize = (((uint64_t)htonl(t1)) << 32) + htonl(t1 >> 32);
	if(send(mysocket, &chunkSize, sizeof(uint64_t), 0) != sizeof(uint64_t)) {
		printf("Failed to send chunkSize\n");
		return EXIT_FAILURE;
	}

	//Sends file size
	uint64_t t2 = atoi(argv[4]);
	uint64_t filesize = (((uint64_t)htonl(t2)) << 32) + htonl(t2 >> 32);
	if(send(mysocket, &filesize, sizeof(uint64_t), 0) != sizeof(uint64_t)) {
		printf("Failed to send file size\n");
		return EXIT_FAILURE;
	}

	//Sends filenames length
	uint64_t t3 = atoi(argv[5]);
	uint64_t filenameLength = (((uint64_t)htonl(t3)) << 32) + htonl(t3 >> 32);
	if(send(mysocket, &filenameLength, sizeof(uint64_t), 0) != sizeof(uint64_t)) {
		printf("Failed to send filename length\n");
		return EXIT_FAILURE;
	}

	//Sends filename
	if(send(mysocket, argv[3], strlen(argv[3]), 0) != atoi(argv[5])) {
		printf("failed to send filename\n");
		return EXIT_FAILURE;
	}

	while ((ch = fgetc(stdin)) != EOF) {
		if(count > MAXBUFLEN) {
			count = 0;
			if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
				printf("Send failed!\n");
				return EXIT_FAILURE;
			}
			free(buffer);
			buffer = calloc(MAXBUFLEN + 1,sizeof(char)); 
		} else {
			buffer[count] = ch;
			count++;
		}
	}
	if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
		printf("Send failed1!\n");
		return EXIT_FAILURE;
	}


	close(mysocket);
	return EXIT_SUCCESS;
}
