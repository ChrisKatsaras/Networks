/**
 * server.c
 * Christopher Katsaras & Connor Geddes
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
#include "transferQueue.h"

#define MAXBUFLEN 4096

void * workerFunc(void * arg) {	

	ThreadArgs * myThreadArg = (ThreadArgs*)arg;
	void* ret = NULL;
	pthread_join(myThreadArg->tid, &ret);

	if(ret != NULL) {
		getTransfer(myThreadArg->q, (char*)ret); // remove from the queue
	}

	pthread_exit(NULL);
}

void * writeFile(void * arg){	
	FILE * fp; //File pointer
	char * buffer = malloc(sizeof(MAXBUFLEN + 1)); //Buffer for transfer
	char * fileName;
	ThreadArgs * myThreadArg = (ThreadArgs*)arg;
	int *connection = &myThreadArg->connection[myThreadArg->spot];
	int len; //Len for transfer
	int chunkSize;
	int fileSize;
	int filenameLength;
	uint64_t t;

	//Gets chunksize
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ((uint64_t) t << 32) + ntohl(t >> 32);
	chunkSize = t;

	//Gets gets file size
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ((uint64_t) t << 32) + ntohl(t >> 32);
	fileSize = t;

	//Gets filename length
	len = recv(*connection, &t, sizeof(uint64_t), 0);
	t = ((uint64_t) t << 32) + ntohl(t >> 32);
	filenameLength = t;
	fileName = malloc(sizeof(char) * (filenameLength + 1));

	//Gets the filename
	len = recv(*connection, fileName, filenameLength, 0);
	fileName[filenameLength] = '\0';

	//For testing purposes
	/*printf("Chunksize: %d\n", chunkSize);
	printf("Filesize: %d\n", fileSize);
	printf("Filename length: %d\n", filenameLength);
	printf("Filename %s\n", fileName);*/

	if(noCollision(myThreadArg->q, fileName)) {
		strcpy(buffer, "1");
		send(*connection, buffer, 1, 0);
		sendTransfer(myThreadArg->q, fileName, fileSize); // adds fileName and fileSize to our queue 
		fp = fopen(fileName ,"a");
		//While there is more data to get
		while(len > 0) {
			buffer = malloc(sizeof(char)*(chunkSize + 1));
			len = recv(*connection, buffer, chunkSize, 0);
			buffer[len] = '\0';
			//printf("%s\n", buffer); //Outputs message "chunk"
			fputs(buffer, fp);
			free(buffer);
		}

		printf("\n\n\n");
		printf("Closing file\n");
		printf("Closing transfer thread!\n");
		close(*connection);
	    pthread_exit((void*)fileName);
		fclose(fp);
	} else {
		strcpy(buffer, "0");
		send(*connection, buffer, 1, 0);
		printf("ERROR FILE_COLLISION: CURRENTLY RUNNING THREAD WITH IDENTICAL FILE_NAME: %s\n", fileName);
		printf("Closing transfer thread!\n");
		close(*connection);
	    pthread_exit(NULL);
	}
}

void * uiThread(void * arg) {
	ThreadArgs * myThreadArg = (ThreadArgs*)arg;
	int * exit = &myThreadArg->exitCond;
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
			printf("\n\n****************\n");
			printf("Active Tranfers\n");
			printf("****************\n\n\n");
			printQueue(myThreadArg->q); 
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
}

void * mainThreadFunc(void * args) {
	ThreadArgs * arg = (ThreadArgs*)args;
	ThreadArgs argVal;
	argVal = *arg;
	char * portNumber = NULL;
	int userSocket; 
	struct sockaddr_in serv;
	struct sockaddr_in client;
	int err;
	pthread_t tids[100];
	pthread_t workerID[100];
	int counter = 0;

	portNumber = (char*)malloc(sizeof(char) * (strlen(arg->portNumber) + 1));
	strcpy(portNumber,arg->portNumber);

	socklen_t socksize = sizeof(struct sockaddr_in);
	
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;                
	serv.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv.sin_port = htons(atoi(portNumber));           
	userSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(userSocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(EXIT_FAILURE);
	}

	listen(userSocket, 1000);

	argVal.connection[counter] = accept(userSocket, (struct sockaddr *)&client, &socksize);
	argVal.spot=counter;
	while(argVal.connection[counter]) {
		//Creates thread for new connection
		err = pthread_create(&tids[counter], NULL, writeFile, (void *)&argVal);
		if (err != 0){
            printf("Cannot create thread, error: %d", err);
            exit(-1);
        } 

        argVal.tid = tids[counter];//Worker
        //creates a thread for the worker function 
        err = pthread_create(&workerID[counter], NULL, workerFunc, (void *)&argVal);
		if (err != 0){
            printf("Cannot create thread, error: %d", err);
            exit(-1);
        }

        //Accepts next connection
        counter++;	
		argVal.connection[counter] = accept(userSocket, (struct sockaddr *)&client, &socksize);
		argVal.spot=counter;
	}

	close(userSocket);
	return 0;
}

int main(int argc, char *argv[]) {

	char * portNumber;
	ThreadArgs arg;
	int err;
	pthread_t mainTID;
	pthread_t uiTID;
	int exitServer = 0;
	TransferQueue *q = createTransferQueue();
	arg.q = q;
	arg.exitCond = exitServer;

	if(argc == 2) {
		portNumber = argv[1];
		arg.portNumber = (char*)malloc(sizeof(char) * (strlen(portNumber) + 1));
		strcpy(arg.portNumber,portNumber);
		printf("Listening on port %s\n", portNumber);
	} else {
		perror("Error: Missing port number!\n");
		exit(EXIT_FAILURE);
	}

	err = pthread_create(&mainTID, NULL, mainThreadFunc, (void *)&arg);
	if (err != 0){
        printf("Cannot create the main thread, error: %d", err);
        exit(-1);
    } else {
    	printf("Main thread created\n");
    }

    err = pthread_create(&uiTID, NULL, uiThread, (void *)&arg);
	if (err != 0){
        printf("Cannot create UI thread, error: %d", err);
        exit(-1);
    } else {
    	printf("UI thread created\n");
    }

    while(!exitServer) {
    	pthread_join(uiTID, NULL);
    	exitServer = 1;
    }

	return 0;
}
