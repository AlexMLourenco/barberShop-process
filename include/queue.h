/*!
 * \file
 * \brief A simple linked list queue (with some extra array like operation).
 *
 * \author Miguel Oliveira e Silva, 2017
 */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct _Item_
{
   struct _Item_* next;
   void* elem;
} Item;

typedef struct _Queue_
{
   int alloc;
   Item* head;
   Item* tail;
   int size;
} Queue;

/**
 *  \brief Creates and/or initializes a new queue.
 *
 *  \details If the argument is NULL, a new queue structure is allocated in heap (using mem_alloc()),
 *  otherwise the data structure registers that allocation was external (preventing it from being
 *  freed on destruction).
 *
 *  \param [in] queue optional pointer to a `Queue`
 *
 *  \return pointer to the new initialized queue (returns queue, if it is not `NULL`).
 */
Queue* new_queue(Queue* queue);

/**
 *  \brief Destroys a queue.
 *
 *  \details If allocation was internal (`NULL` argument on new_queue()), it will also free
 *  the Queue pointer returning `NULL`; otherwise it will return the queue pointer.
 *
 *  \param [in] queue pointer to a `Queue`
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *  </DL>
 *
 *  \return the value of queue, if allocation was external, otherwise is returns `NULL`.
 */
Queue* destroy_queue(Queue* queue);

/**
 *  \brief Adds a new element at the end of the queue.
 *
 *  \param [in] queue pointer to a `Queue`
 *  \param [in] elem pointer to the element
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD><BR>
 *     <DD><code>elem != NULL</code></DD>
 *  </DL>
 */
void in_queue(Queue* queue, void* elem);

/**
 *  \brief Removes and returns the oldest element of the queue.
 *
 *  \param [in] queue pointer to a `Queue`
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *  </DL>
 *
 *  \return the remove element pointer.
 */
void* out_queue(Queue* queue);

/**
 *  \brief Is the queue empty?
 *
 *  \param [in] queue pointer to a `Queue`
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *  </DL>
 *
 *  \return true (`!=0`) if empty, false (`0`) otherwise
 */
int empty_queue(Queue* queue);

/**
 *  \brief Number of elements in the queue.
 *
 *  \param [in] queue pointer to a `Queue`
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *  </DL>
 *
 *  \return number of elements in the queue.
 */
int size_queue(Queue* queue);

/**
 *  \brief Get any element of the queue.
 *
 *  \details Indexes starts at the head (oldest) element of the queue.
 *
 *  \param [in] queue pointer to a `Queue`
 *  \param [in] idx index of the element in the queue
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *     <DD><code>idx >= 0 && idx < size_queue(queue)</code></DD><BR>
 *  </DL>
 *
 *  \return the idx's element of the queue.
 */
void* get_queue(Queue* queue, int idx);

/**
 *  \brief Remove the idx's element of the queue.
 *
 *  \details *  Indexes starts at the head (oldest) element of the queue.
 *
 *  \param [in] queue pointer to a `Queue`
 *  \param [in] idx index of the element in the queue
 *
 *  <DL><DT><B>Precondition:</B></DT>
 *     <DD><code>queue != NULL</code></DD>
 *     <DD><code>idx >= 0 && idx < size_queue(queue)</code></DD><BR>
 *  </DL>
 *
 *  \return the pointer of the removed element of the queue.
 */
void* remove_queue(Queue* queue, int idx);

#endif
