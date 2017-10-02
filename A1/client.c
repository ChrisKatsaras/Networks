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
#include <time.h>
 
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[]) {
	int MAXBUFLEN = 10000;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	//FILE *fp = fopen(stdin, "r");
	char * buffer = calloc(MAXBUFLEN + 1,sizeof(char));
	char hostName[100] = "localhost";
	char portString[100];
	char ipAddr[INET6_ADDRSTRLEN];
	char * temp;
	int rv;
	int mysocket;
	struct sockaddr_in dest;

 	if(argc != 2 && argc != 3) {
 		perror("Wrong number of arguements!");
 		EXIT_FAILURE;
 	} else {
 		temp = strtok(argv[1],":");
 		if(temp != NULL) {
 			strcpy(hostName, temp);
	 		temp = strtok(NULL, ":");
	 		strcpy(portString,temp);
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

 	if ((rv = getaddrinfo(hostName, portString, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)servinfo->ai_addr), ipAddr, sizeof ipAddr);
    //printf("The IP is %s\n",ipAddr);
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	memset(&dest, 0, sizeof(dest));
	
	dest.sin_family = AF_INET;
	//dest.sin_addr.s_addr = inet_addr(ipAddr); 
	inet_aton(ipAddr, &(dest.sin_addr));
	dest.sin_port = htons(atoi(portString));               
 	
 	//Connects to server
	if(connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in)) != 0) {
		perror("Client unable to connect to server");
		exit(EXIT_FAILURE);
	}

	char ch;
	int count = 0;
	clock_t begin = clock();
	while ((ch = fgetc(stdin)) != EOF) {
		if(count > MAXBUFLEN) {
			count = 0;
			//sleep(1);
			if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
					printf("Send failed!\n");
			}

			free(buffer);
			buffer = calloc(MAXBUFLEN + 1,sizeof(char)); 
		} else {
			buffer[count] = ch;
			count++;
		}
		//printf("%c", ch);
	}

	if(send(mysocket, buffer, strlen(buffer), 0) != strlen(buffer)) {
		printf("Send failed!\n");
	}
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%lf\n", time_spent);


	//Needs to break the message into smaller chunks
	//if(strlen(msg) > MAXBUFLEN) {
		/*int j;
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
		}*/

		
	//} else {
		//send(mysocket, msg, strlen(msg), 0); 
	//}

	/*len = recv(mysocket, buffer, MAXBUFLEN, 0);
 	
	buffer[len] = '\0';
 
	printf("Received %s (%d bytes).\n", buffer, len);*/
 
	close(mysocket);
	return EXIT_SUCCESS;
}
