#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/queue.h"

/* Initialize the circular queue */
void init_queue(Queue *q, int capacity) {
    if (capacity <= 0) {
        capacity = DEFAULT_QUEUE_CAPACITY;
    }
    q->song_ids = (int *)malloc(sizeof(int) * capacity);
    if (q->song_ids == NULL) {
        printf("[Error] Memory allocation failed for Upcoming Queue!\n");
        q->front = 0;
        q->rear = -1;
        q->count = 0;
        q->capacity = 0;
        return;
    }
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->capacity = capacity;
}

/* Free allocated queue memory */
void free_queue(Queue *q) {
    if (q->song_ids != NULL) {
        free(q->song_ids);
        q->song_ids = NULL;
    }
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->capacity = 0;
}

/* Clear all elements from queue */
void clear_queue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

/* Check if queue is empty */
int is_queue_empty(const Queue *q) {
    return (q->count == 0);
}

/* Check if queue is full */
int is_queue_full(const Queue *q) {
    return (q->count >= q->capacity);
}

/* Enqueue a song at the rear (FIFO) */
int enqueue(Queue *q, int song_id) {
    if (is_queue_full(q)) {
        printf("[Warning] Upcoming Queue is full! Cannot add more songs.\n");
        return 0;
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->song_ids[q->rear] = song_id;
    q->count++;
    return 1;
}

/* Dequeue the song from the front (FIFO) */
int dequeue(Queue *q, int *out_song_id) {
    if (is_queue_empty(q)) {
        return 0; /* Queue is empty */
    }

    if (out_song_id != NULL) {
        *out_song_id = q->song_ids[q->front];
    }

    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return 1;
}

/* Peek at the next upcoming song without dequeuing */
int peek_queue(const Queue *q, int *out_song_id) {
    if (is_queue_empty(q)) {
        return 0;
    }
    if (out_song_id != NULL) {
        *out_song_id = q->song_ids[q->front];
    }
    return 1;
}

/* Remove a specific song ID from the queue (used during cascade deletion) */
int remove_song_from_queue(Queue *q, int song_id) {
    if (is_queue_empty(q)) {
        return 0;
    }

    int *temp = (int *)malloc(sizeof(int) * q->capacity);
    if (temp == NULL) {
        return 0;
    }

    int new_count = 0;
    int idx = q->front;

    for (int i = 0; i < q->count; i++) {
        if (q->song_ids[idx] != song_id) {
            temp[new_count] = q->song_ids[idx];
            new_count++;
        }
        idx = (idx + 1) % q->capacity;
    }

    int removed = q->count - new_count;

    /* Re-populate queue */
    for (int i = 0; i < new_count; i++) {
        q->song_ids[i] = temp[i];
    }
    free(temp);

    q->front = 0;
    q->rear = new_count - 1;
    q->count = new_count;

    return removed;
}

/* Display all upcoming songs in order */
void display_queue(const Queue *q, const Library *lib) {
    printf("\n============================================================\n");
    printf("                    UPCOMING SONGS QUEUE                    \n");
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-25s | %-18s | %-6s\n", "Pos", "ID", "Title", "Artist", "Dur.");
    printf("------+------+---------------------------+--------------------+--------\n");

    if (is_queue_empty(q)) {
        printf(" [Queue is currently empty. Songs will play from playlist]\n");
        printf("============================================================\n\n");
        return;
    }

    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        int sid = q->song_ids[idx];
        Song *s = find_song_by_id(lib, sid);

        if (s != NULL) {
            char dur[16];
            format_duration(s->duration, dur);

            char short_title[26];
            char short_artist[19];
            strncpy(short_title, s->title, 25);
            short_title[25] = '\0';
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-25s | %-18s | %-6s\n",
                   i + 1, s->id, short_title, short_artist, dur);
        } else {
            printf(" %-4d | %-4d | [Song Not in Library]    | -                  | -\n",
                   i + 1, sid);
        }

        idx = (idx + 1) % q->capacity;
    }

    printf("============================================================\n");
    printf("Total Songs in Upcoming Queue: %d / %d\n\n", q->count, q->capacity);
}
