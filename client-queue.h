/**
 *  \brief An array based client queue for barber shop.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef CLIENT_QUEUE_H
#define CLIENT_QUEUE_H

#include "global.h"

#define MAX_CLIENT_QUEUE_SIZE MAX_CLIENT_BENCHES_SEATS

typedef struct _RQItem_
{
   int clientID;
   int benchPos;
   int request;
   int order;
} RQItem;

typedef struct _ClientQueue_
{
   RQItem array[MAX_CLIENT_QUEUE_SIZE];
   int head; // oldest item position
   int tail; // first free position
   int size;
   int waitingQueueNumber;
} ClientQueue;

RQItem empty_item();
void init_client_queue(ClientQueue* queue);
void term_client_queue(ClientQueue* queue);
int terminated_client_queue(ClientQueue* queue);
int in_client_queue(ClientQueue* queue, RQItem item);
RQItem out_client_queue(ClientQueue* queue);
int empty_client_queue(ClientQueue* queue);
int full_client_queue(ClientQueue* queue);
int size_client_queue(ClientQueue* queue);

#endif
