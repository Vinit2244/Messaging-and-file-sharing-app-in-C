#include "queue.h"

// Initialise an empty queue
queue init_queue()
{
    queue Q = (queue) calloc(1, sizeof(st_queue));

    if (Q == NULL)
    {
        fprintf(stderr, RED("calloc : Unable to allocate memory to queue : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&Q->lock, NULL) != 0)
    {
        // Unable to initialise the lock
        fprintf(stderr, RED("pthread_mutex_init : Unable to initialise queue lock : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    Q->n_nodes = 0;
    Q->first_node = NULL;
    Q->last_node = NULL;

    return Q;
}

// Initialises a new queue node with the given specifics
queue_node init_queue_node()
{
    queue_node n = (queue_node) calloc(1, sizeof(st_queue_node));

    if (n == NULL)
    {
        fprintf(stderr, RED("calloc : Unable to allocate memory to queue node : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    n->prev_node = NULL;
    n->next_node = NULL;

    return n;
}

// Inserts a new node at the end of the queue
void enqueue(queue q)
{
    queue_node n = init_queue_node();

    pthread_mutex_lock(&q->lock);
    if (q->n_nodes == 0)
    {
        q->first_node = n;
        q->last_node = n;
    }
    else
    {
        n->prev_node = q->last_node;
        q->last_node->next_node = n;
        q->last_node = n;
    }
    q->n_nodes++;
    pthread_mutex_unlock(&q->lock);
}

// Dequeues the first element in the queue (returns 0 if successfull else 1 if some error occurs)
int dequeue(queue q)
{
    pthread_mutex_lock(&q->lock);
    if (q->n_nodes <= 0)
    {
        fprintf(stderr, RED("Can't dequeue from queue as queue is already empty\n"));
        return 1;
    }
    else if (q->n_nodes == 1)
    {
        free(q->first_node);
        q->first_node = NULL;
        q->last_node = NULL;
    }
    else
    {
        queue_node node_to_free = q->first_node;
        q->first_node = q->first_node->next_node;
        q->first_node->prev_node = NULL;

        free(node_to_free);
    }
    q->n_nodes--;
    pthread_mutex_unlock(&q->lock);

    return 0;
}

// 0 if successfully removed, 1 otherwise
int remove_particular_node(queue q)
{
    if (q->n_nodes <= 0)
    {
        fprintf(stderr, RED("Can't remove node from queue as queue is already empty\n"));
        return 1;
    }

    queue_node trav = q->first_node;
    while (trav != NULL)
    {
        // Write the code to find the required node
        // if trav is the node we are finding then write the following code
        // queue_node p_node = trav->prev_node;
        // queue_node n_node = trav->next_node;
        // if (p_node == NULL && n_node == NULL)
        // {
        //     q->first_node = NULL;
        //     q->last_node = NULL;
        // }
        // else if (p_node == NULL && n_node != NULL)
        // {
        //     n_node->prev_node = NULL;
        //     q->first_node = n_node;
        // }
        // else if (p_node != NULL && n_node == NULL)
        // {
        //     p_node->next_node = NULL;
        //     q->last_node = p_node;
        // }
        // else if (p_node != NULL && n_node != NULL)
        // {
        //     p_node->next_node = n_node;
        //     n_node->prev_node = p_node;
        // }
        // free(trav);
        // q->n_nodes--;
        // return 0;

        trav = trav->next_node;
    }

    return 1;
}

// Frees the queue head and all the corresponding nodes if any remaining
void destroy_queue(queue q)
{
    queue_node trav = q->first_node;
    while (trav != NULL)
    {
        queue_node node_to_free = trav;
        trav = trav->next_node;
        free(node_to_free);
    }
    free(q);
    return;
}

