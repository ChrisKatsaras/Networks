#include "transferQueue.h"

TransferQueue *createTransferQueue(void) {
	TransferQueue *q = (TransferQueue*)malloc(sizeof(TransferQueue));
	q->tail = q->head = NULL;


	pthread_mutex_init(&q->mutex, NULL);

	pthread_cond_init(&q->cond, NULL);

	return q;
}

TransferNode *initNode(char *fileName, int fileSize) {
	TransferNode *newTrans = (TransferNode*)malloc(sizeof(TransferNode));
	newTrans->tran.fileName = (char*)malloc(sizeof(char) * (strlen(fileName) + 1));
	strcpy(newTrans->tran.fileName, fileName);
	newTrans->tran.fileSize = fileSize;
	newTrans->next = NULL;

	return newTrans;
}

void sendTransfer(TransferQueue *q, char *fileName, int fileSize) {
	TransferNode *node = initNode(fileName, fileSize);
	pthread_mutex_lock(&q->mutex);

	if(q->tail != NULL) {
		q->tail->next = node;
		q->tail = node;
	} else {
		q->tail = q->head = node;
	}

	pthread_cond_signal(&q->cond);
	pthread_mutex_unlock(&q->mutex);

	//fprintf(stderr, "%s\n", );
}

void getTransfer(TransferQueue *q, char *fileName) {
	pthread_mutex_lock(&q->mutex);

	while(q->head == NULL) {
		pthread_cond_wait(&q->cond, &q->mutex);
	}

    TransferNode *tempNode = NULL;
    TransferNode *currNode = NULL;
    if(strcmp(q->head->tran.fileName, fileName) == 0) {
        tempNode = q->head;
        q->head = tempNode->next;
        if(tempNode->next == NULL) {
            q->tail = NULL;
        }

        free(tempNode);

    }
    else {
        
        currNode = q->head;

        while(currNode->next != NULL && strcmp(currNode->tran.fileName, fileName) != 0) {
            tempNode = currNode;
            currNode = currNode->next;


        }
        if(currNode->next == NULL) {

            q->tail = tempNode;

        }

        tempNode->next = currNode->next;
        free(currNode);   
    }
    pthread_mutex_unlock(&q->mutex);
}
