#ifndef QUEUE_H
#define QUEUE_H

#include "library.h"

#define DEFAULT_QUEUE_CAPACITY 30

/* Circular FIFO Queue for upcoming songs */
typedef struct Queue {
    int *song_ids;      /* Array of song IDs waiting in queue */
    int front;          /* Index of front element */
    int rear;           /* Index of rear element */
    int count;          /* Current number of queued songs */
    int capacity;       /* Maximum capacity of queue */
} Queue;

/* Lifecycle functions */
void init_queue(Queue *q, int capacity);
void free_queue(Queue *q);
void clear_queue(Queue *q);

/* Queue operations */
int is_queue_empty(const Queue *q);
int is_queue_full(const Queue *q);
int enqueue(Queue *q, int song_id);
int dequeue(Queue *q, int *out_song_id);
int peek_queue(const Queue *q, int *out_song_id);
int remove_song_from_queue(Queue *q, int song_id);

/* Display */
void display_queue(const Queue *q, const Library *lib);

#endif /* QUEUE_H */
