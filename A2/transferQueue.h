#ifndef __TRANS_QUEUE__
#define __TRANS_QUEUE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
	char *fileName;
	int fileSize;
} Transfer;

typedef struct transfer_node{
	Transfer tran;
	struct transfer_node *next;
} TransferNode;

typedef struct {
	TransferNode *head;
	TransferNode *tail;

	pthread_mutex_t mutex;

	pthread_cond_t cond;
} TransferQueue;

typedef struct {
	int exitCond;
	int connection[1000];
	pthread_t tid;
	int spot;
	char * portNumber;
	TransferQueue *q;
} ThreadArgs;

/*
* Function: createTransferQueue
*
* Param: void
* Desc: creates and allocates memory for a queue
* Ret: type TransferQueue*, returns an allocated queue
*/
TransferQueue *createTransferQueue(void);

/*
* Function: initNode
*
* Param: string of the fileName and the size of the fileName
* Desc: allocates memory for each node and the fileName 
* Ret: type TransferNode*, returns the newly allocated node
*/
TransferNode *initNode(char *fileName, int fileSize);

/*
* Function: sendTransfer 
*
* Param: pointer to the queue, string of the fileName to be added and size of the fileName
* Desc: This will add the fileName and fileSize to the queue
* Ret: type void
*/
void sendTransfer(TransferQueue *q, char *fileName, int fileSize);

/*
* Function: getTransfer 
*
* Param: pointer to the queue and a string containing the filename to be deleted
* Descr: will go through the queue and find where the filename is in the queue
* Ret: type void
*/
void getTransfer(TransferQueue *q, char* fileName);

/*
* Function noCollision
*
* Param: pointer to the queue and a string of the fileName to be added
* Descr: will check if a fileName trying to be added is already in the queue
* Ret: type void, returns true if the fileName being added to the queue is unique and false otherwise
*/
bool noCollision(TransferQueue *q, char *newFile);

/*
* Function printQueue
*
* Param: pointer to the queue
* Descr: will go through the queue and print each node
* Ret: type void
*/
void printQueue(TransferQueue *q);

#endif