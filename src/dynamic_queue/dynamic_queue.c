/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  dynamic_queue.c
 * @brief This file contains the implementation of the dynamic queue data structure.
 * 
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../utils/utils.h"
#include "dynamic_queue.h"

#define INITIAL_CAPACITY 10

Queue* queue_create()
{
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL)
    {
        LOG(ERROR, PARAMS, "Memory allocation failed for queue_create queue struct");
        return NULL;
    }
    queue->array = (void**)malloc(INITIAL_CAPACITY * sizeof(void *));
    if (queue->array == NULL)
    {
        LOG(ERROR, PARAMS, "Memory allocation failed for queue_create queue array");
        free(queue);
        return NULL;
    }
    queue->front = -1;
    queue->rear = -1;
    queue->capacity = INITIAL_CAPACITY;
    return queue;
}

int queue_is_empty(Queue *queue)
{
    return queue->front == -1;
}

int queue_enqueue(Queue *queue, void *item)
{
    int i = 0;
    int j = queue->front;
    if ((queue->rear + 1) % queue->capacity == queue->front) 
	{
        /* Queue is full, need to resize*/
        int newCapacity = queue->capacity * 2;
        void **newArray = (void**)malloc(newCapacity * sizeof(void *));
        if (newArray == NULL) {
            LOG(ERROR, PARAMS, "Memory allocation failed");
            return ERR;
        }
        
        while (j != queue->rear) {
            newArray[i++] = queue->array[j];
            j = (j + 1) % queue->capacity;
        }
        newArray[i] = queue->array[j];
        free(queue->array);
        queue->array = newArray;
        queue->front = 0;
        queue->rear = i;
        queue->capacity = newCapacity;
    }
    if (queue_is_empty(queue))
        queue->front = 0;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;

	return SUCCESS;
}

void* queue_dequeue(Queue *queue)
{
	void *item = queue->array[queue->front];
    if (queue_is_empty(queue)) {
        LOG(ERROR, PARAMS, "Trying to dequeue from an empty queue.");
        return NULL;
    }
   
    if (queue->front == queue->rear)
        queue->front = queue->rear = -1;
    else
        queue->front = (queue->front + 1) % queue->capacity;
    return item;
}

void queue_display(Queue *queue)
{
	int i = queue->front;
    if (queue_is_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }
    printf("Queue: ");
    do {
        printf("%p ", queue->array[i]);
        i = (i + 1) % queue->capacity;
    } while (i != (queue->rear + 1) % queue->capacity);
    printf("\n");
}

void queue_clear(Queue *queue, FreeFunc freeFunc)
{
    if (queue == NULL) {
        return;
    }
    while (!queue_is_empty(queue)) {
        freeFunc(queue_dequeue(queue));
    }

    queue->front = -1;
    queue->rear = -1;

}

void queue_destroy(Queue *queue, FreeFunc freeFunc)
{
    int i = 0;
    if (queue == NULL) {
        return;
    }
    i = queue->front;

    while (!queue_is_empty(queue) && i != (queue->rear + 1) % queue->capacity) {
        freeFunc(queue->array[i]);
        i = (i + 1) % queue->capacity;
    }
    free(queue->array);
    free(queue);
}

