#include <stdio.h>
#include <stdlib.h>
#include "dbc.h"
#include "client-queue.h"

static RQItem empty = {0,-1,0,0};

RQItem empty_item()
{
   return empty;
}

void init_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");

   for(int i = 0; i < MAX_CLIENT_QUEUE_SIZE; i++)
      queue->array[i] = empty;
   queue->head = 0;
   queue->tail = 0;
   queue->size = 0;
   queue->waitingQueueNumber = 0;
}

void term_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");

}

int in_client_queue(ClientQueue* queue, RQItem item)
{
   require (queue != NULL, "queue argument required");
   require (!full_client_queue(queue), "client queue is full");

   int res;
   queue->waitingQueueNumber++;
   res = item.order = queue->waitingQueueNumber;
   queue->array[queue->tail] = item;
   queue->tail = (queue->tail+1) % MAX_CLIENT_QUEUE_SIZE;
   queue->size++;
   return res;
}

RQItem out_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");
   require (!empty_client_queue(queue), "client queue is empty");

   RQItem res = queue->array[queue->head];
   queue->array[queue->head] = empty;
   queue->head = (queue->head+1) % MAX_CLIENT_QUEUE_SIZE;
   queue->size--;

   return res;
}

int empty_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");

   return queue-> size == 0;
}

int full_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");

   return queue-> size == MAX_CLIENT_QUEUE_SIZE;
}

int size_client_queue(ClientQueue* queue)
{
   require (queue != NULL, "queue argument required");

   return queue-> size;
}

