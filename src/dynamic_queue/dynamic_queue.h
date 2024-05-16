/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file dynamic_queue.h
 * @brief Header file for a dynamic-size queue implementation in C.
 * 
 *********************************************************************/

#ifndef DYNAMIC_QUEUE_H
#define DYNAMIC_QUEUE_H

 /* @brief Structure representing a dynamic-size queue. */
typedef struct {
    void **array; /**< Array to store queue elements */
    int front; /**< Index of the front element */
    int rear; /**< Index of the rear element */
    int capacity; /**< Current capacity of the queue */
} Queue;

/**
 * @brief Function pointer type for freeing an element.
 */
typedef void (*FreeFunc)(void *);

/**
 * @brief Creates a new dynamic-size queue.
 * @return A pointer to the newly created queue.
 * @remark Complexity: O(1).
 */
Queue* queue_create();

/**
 * @brief Checks if the queue is empty.
 * @param queue A pointer to the queue.
 * @return 1 if the queue is empty, 0 otherwise.
 * @remark Complexity: O(1).
 */
int queue_is_empty(Queue *queue);

/**
 * @brief Adds an element to the rear of the queue.
 * @param queue A pointer to the queue.
 * @param item A pointer to the item to be added to the queue.
 * @remark Complexity: O(1) amortized time, O(n) worst-case time when resizing.
 * @return 1 if the element was successfully added, 0 otherwise.
 */
int queue_enqueue(Queue *queue, void *item);

/**
 * @brief Removes and returns the element at the front of the queue.
 * @param queue A pointer to the queue.
 * @return A pointer to the element at the front of the queue.
 * @remark Complexity: O(1).
 */
void* queue_dequeue(Queue *queue);

/**
 * @brief Displays the elements of the queue.
 * @param queue A pointer to the queue.
 * @remark Complexity: O(n).
 */
void queue_display(Queue *queue);

/**
 * @brief Clears the queue.
 * @param queue A pointer to the queue.
 * @remark Complexity: O(1).
 */
void queue_clear(Queue *queue, FreeFunc freeFunc);

/**
 * @brief Destroys the dynamic queue and frees the allocated memory.
 * 
 * This function deallocates the memory used by the dynamic queue and sets the queue pointer to NULL.
 * After calling this function, the queue should not be used anymore.
 * @remark Complexity: O(1).
 *
 * @param queue A pointer to the dynamic queue to be destroyed.
 */
void queue_destroy(Queue *queue, FreeFunc freeFunc);

#endif /* DYNAMIC_QUEUE_H */
