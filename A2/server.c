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
#include <pthread.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>

#define MAXBUFLEN 4096

void * writeFile(void * arg){	
	FILE * fp; //File pointer
	char * buffer = malloc(sizeof(MAXBUFLEN + 1)); //Buffer for transfer
	char * fileName;
	int * connection = (int*)arg; //Connection var for socket
	int len; //Len for transfer
	int chunkSize;
	int fileSize;
	int filenameLength;
	uint64_t t;

	//Gets chunksize
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ntohll(t);
	chunkSize = t;

	//Gets gets file size
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ntohll(t);
	fileSize = t;

	//Gets filename length
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ntohll(t);
	filenameLength = t;
	fileName = malloc(sizeof(filenameLength) + 1);

	//Gets the filename
	len = recv(*connection, fileName, filenameLength, 0);
	fileName[filenameLength] = '\0';

	printf("Chunksize: %d\n", chunkSize);
	printf("Filesize: %d\n", fileSize);
	printf("Filename length: %d\n", filenameLength);
	printf("Filename %s\n", fileName);

	//TODO: Check filename for collision

	fp = fopen(fileName ,"a");

	//While there is more data to get
	while(len > 0) {
		buffer = calloc(MAXBUFLEN + 1,sizeof(char)); 
		len = recv(*connection, buffer, MAXBUFLEN, 0);
		buffer[len] = '\0';
		printf("%s\n", buffer); //Outputs message "chunk"
		fputs(buffer, fp);
		free(buffer);
	}
	
	printf("\n\n\n");
	printf("Closing file\n"); 
	fclose(fp);
	close(*connection);
	pthread_exit(NULL);
    return NULL;
}

void * uiThread(void * arg) {	
	int * exit = (int *) arg;
	int userOption;
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);

	while(*exit == 0) {

		printf("Choose an option:\n");
		printf("1: Show active transfers\n");
		printf("2: Shut down server\n");
		scanf("%d", &userOption);
		getchar(); //In case the user enters a char

		if(userOption == 1) {
			//Show active transfers
		} else if(userOption == 2) {
			pthread_mutex_lock(&lock); //Locks
			*exit = 1;//Shuts down server
			pthread_mutex_unlock(&lock); //Unlocks
		} else {
			printf("Invalid choice!\n");
		}
	}
	printf("Exiting UI thread\n");
	pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {

	char * portNumber;
	int userSocket; 
	struct sockaddr_in serv;
	struct sockaddr_in client;
	int err;
	pthread_t tid;
	int exitServer = 0;

	//threadData * data = malloc(sizeof(threadData));
	
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

	err = pthread_create(&tid, NULL, uiThread, (void *)&exitServer);
	if (err != 0){
        printf("Cannot create UI thread, error: %d", err);
        exit(-1);
    } else {
    	printf("UI thread created\n");
    }

	int connection = accept(userSocket, (struct sockaddr *)&client, &socksize);

	while(connection && exitServer == 0) {
		//Creates thread for new connection
		err = pthread_create(&tid, NULL, writeFile, (void *)&connection);
		if (err != 0){
            printf("Cannot create thread, error: %d", err);
            exit(-1);
        } else {
        	printf("Thread created\n");
        }

        //Accepts next connection
		connection = accept(userSocket, (struct sockaddr *)&client, &socksize);
	}

	close(userSocket);
	return 0;
}