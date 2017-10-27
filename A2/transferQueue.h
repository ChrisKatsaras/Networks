#ifndef __TRANS_QUEUE__
#define __TRANS_QUEUE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
	int spot;
	TransferQueue *q;
} ThreadArgs;


TransferQueue *createTransferQueue(void);

TransferNode *initNode(char *fileName, int fileSize);

void sendTransfer(TransferQueue *q, char *fileName, int fileSize);

int getTransfer(TransferQueue *q, Transfer *msg_out);

void *workerFun(void *arg);

#endif