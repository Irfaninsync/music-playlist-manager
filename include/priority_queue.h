#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "library.h"

#define DEFAULT_PQ_CAPACITY 30

/* Element in the max-heap priority queue */
typedef struct PQElement {
    int song_id;            /* Reference to song in library */
    int priority;           /* Priority score: 1 to 10 (10 = highest) */
    long sequence_num;      /* Tie-breaker: earlier queued songs play first on equal priority */
} PQElement;

/* Max-Heap Priority Queue */
typedef struct PriorityQueue {
    PQElement *heap;        /* Flat array representing complete binary tree */
    int size;               /* Current number of elements in heap */
    int capacity;           /* Maximum capacity */
    long counter;           /* Incremented on each insert for FIFO tie-breaking */
} PriorityQueue;

/* Lifecycle functions */
void init_priority_queue(PriorityQueue *pq, int capacity);
void free_priority_queue(PriorityQueue *pq);
void clear_priority_queue(PriorityQueue *pq);

/* Max-Heap Operations */
int pq_is_empty(const PriorityQueue *pq);
int pq_is_full(const PriorityQueue *pq);
int pq_insert(PriorityQueue *pq, int song_id, int priority);
int pq_extract_max(PriorityQueue *pq, int *out_song_id, int *out_priority);
int pq_peek(const PriorityQueue *pq, int *out_song_id, int *out_priority);
int pq_remove_song(PriorityQueue *pq, int song_id);

/* Display */
void pq_display(const PriorityQueue *pq, const Library *lib);

#endif /* PRIORITY_QUEUE_H */
