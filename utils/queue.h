#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "../common_headers/color_codes.h"

// ====================== Queue Structures ======================
typedef struct st_queue_head
{
    pthread_mutex_t lock;
    int n_nodes;
    struct st_queue_node* first_node;
    struct st_queue_node* last_node;
} st_queue_head;

typedef st_queue_head st_queue;
typedef st_queue_head* queue_head;
typedef st_queue_head* queue;

typedef struct st_queue_node
{
    struct st_queue_node* prev_node;
    struct st_queue_node* next_node;
} st_queue_node;

typedef st_queue_node* queue_node;

// ====================== Queue Functions ======================
queue init_queue();                     // Initialises and returns an empty queue
queue_node init_queue_node();           // Initialises a new queue node with the given specifics
void enqueue(queue q);                  // Inserts a new node at the end of the queue
int remove_particular_node(queue q);    // Removes a particular node from the queue
void destroy_queue(queue q);            // Destroys queue head

#endif
