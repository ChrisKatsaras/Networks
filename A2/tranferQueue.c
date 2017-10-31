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

}

void getTransfer(TransferQueue *q, char *fileName) {
	pthread_mutex_lock(&q->mutex); // locks the queue
	while(q->head == NULL) {
		pthread_cond_wait(&q->cond, &q->mutex); 
	}

    TransferNode *tempNode = NULL;
    TransferNode *currNode = NULL;
    // checks if the fileName is in the first Node
    if(strcmp(q->head->tran.fileName, fileName) == 0)
    {
        tempNode = q->head; // saves the location of the node to delete
        q->head = tempNode->next;
        if(tempNode->next == NULL)
        {
            q->tail = NULL;
        }

        free(tempNode); // frees the memory of the node

    }
    else
    {  
        currNode = q->head;

        // goes through the list and saves the previous of the node we want to find
        // to the previous location and the next node to previous node
        while(currNode->next != NULL && strcmp(currNode->tran.fileName, fileName) != 0)
        {
            tempNode = currNode;
            currNode = currNode->next;
        }
        if(currNode->next == NULL)
        {
            q->tail = tempNode;
        }

        tempNode->next = currNode->next;
        free(currNode); // frees the memory of what we want to delete    
    }
    pthread_mutex_unlock(&q->mutex); // unlocks the queue
}


void printQueue(TransferQueue *q) {

	TransferNode *tempNode = q->head;

	// if the queue is empty it will print this
	if(tempNode == NULL) {
		printf("No files being transferred\n\n\n\n");
	}
	int tranNum = 1;

	// will print the transfer numbers fileNames and size of each node
	while(tempNode != NULL) {
		printf("Transfer number: %d\tFile name: %s\tFile size: %d\n\n\n", tranNum, tempNode->tran.fileName, tempNode->tran.fileSize);

		tempNode = tempNode->next;
		tranNum++;
	}
}


bool noCollision(TransferQueue *q, char *newFile) {

	if(q == NULL) {
		return true;
	}
	TransferNode *tempNode = q->head;

	while(tempNode != NULL) {
		if(strcmp(newFile, tempNode->tran.fileName) == 0) {
			//collision detected here
			return false;
		}

		tempNode = tempNode->next;
	}

	return true;
}

