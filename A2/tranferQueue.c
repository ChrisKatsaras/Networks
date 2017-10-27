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

int getTransfer(TransferQueue *q, Transfer *tran_out) {
	int success = 0;

	pthread_mutex_lock(&q->mutex);


	while(q->head == NULL) {
		pthread_cond_wait(&q->cond, &q->mutex);
	}

	TransferNode *oldHead = q->head;
	*tran_out = oldHead->tran; 
	q->head = oldHead->next;
	if(q->head == NULL) {
		q->tail = NULL;
	}

	free(oldHead);
	success = 1;

	pthread_mutex_unlock(&q->mutex);

	return success;
}

void *workerFun(void *arg) {
	ThreadArgs *args = (ThreadArgs*)arg;

	int i = 0;

	char *fileNames[10] = {"fileName1", "fileName2", "fileName3", "filename4", "fileName5", "fileName6", "fileName7", "filename8", "fileName9", "fileName10"};
	int fileSizes[10] = {123, 315, 32532, 32324, 1224, 23424, 1, 2342, 7653, 2345};
	for(i = 0; i < 10; i++) {
		sendTransfer(args->q, fileNames[i], fileSizes[i]);
	}

	return NULL;
}
