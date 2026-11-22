#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/priority_queue.h"

/* Helper to compare priority between two elements */
static int is_higher_priority(const PQElement *a, const PQElement *b) {
    if (a->priority != b->priority) {
        return a->priority > b->priority;
    }
    /* Equal priority: FIFO tie-breaker (smaller sequence number came earlier) */
    return a->sequence_num < b->sequence_num;
}

/* Swap two elements in the heap array */
static void swap_elements(PQElement *a, PQElement *b) {
    PQElement temp = *a;
    *a = *b;
    *b = temp;
}

/* Heapify Up (Bubbling up after insertion): O(log N) */
static void heapify_up(PriorityQueue *pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (is_higher_priority(&pq->heap[index], &pq->heap[parent])) {
            swap_elements(&pq->heap[index], &pq->heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Heapify Down (Sinking down after extracting max): O(log N) */
static void heapify_down(PriorityQueue *pq, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < pq->size && is_higher_priority(&pq->heap[left], &pq->heap[largest])) {
            largest = left;
        }
        if (right < pq->size && is_higher_priority(&pq->heap[right], &pq->heap[largest])) {
            largest = right;
        }

        if (largest != index) {
            swap_elements(&pq->heap[index], &pq->heap[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

/* Initialize priority queue */
void init_priority_queue(PriorityQueue *pq, int capacity) {
    if (capacity <= 0) {
        capacity = DEFAULT_PQ_CAPACITY;
    }
    pq->heap = (PQElement *)malloc(sizeof(PQElement) * capacity);
    if (pq->heap == NULL) {
        printf("[Error] Memory allocation failed for Priority Queue!\n");
        pq->size = 0;
        pq->capacity = 0;
        pq->counter = 0;
        return;
    }
    pq->size = 0;
    pq->capacity = capacity;
    pq->counter = 0;
}

/* Free priority queue */
void free_priority_queue(PriorityQueue *pq) {
    if (pq->heap != NULL) {
        free(pq->heap);
        pq->heap = NULL;
    }
    pq->size = 0;
    pq->capacity = 0;
}

/* Clear priority queue */
void clear_priority_queue(PriorityQueue *pq) {
    pq->size = 0;
    pq->counter = 0;
}

/* Check if empty */
int pq_is_empty(const PriorityQueue *pq) {
    return (pq->size == 0);
}

/* Check if full */
int pq_is_full(const PriorityQueue *pq) {
    return (pq->size >= pq->capacity);
}

/* Insert element into max-heap: O(log N) */
int pq_insert(PriorityQueue *pq, int song_id, int priority) {
    if (pq_is_full(pq)) {
        printf("[Warning] Priority Playback Queue is full!\n");
        return 0;
    }

    pq->heap[pq->size].song_id = song_id;
    pq->heap[pq->size].priority = priority;
    pq->heap[pq->size].sequence_num = ++(pq->counter);

    heapify_up(pq, pq->size);
    pq->size++;
    return 1;
}

/* Extract root (highest priority song): O(log N) */
int pq_extract_max(PriorityQueue *pq, int *out_song_id, int *out_priority) {
    if (pq_is_empty(pq)) {
        return 0;
    }

    if (out_song_id != NULL) {
        *out_song_id = pq->heap[0].song_id;
    }
    if (out_priority != NULL) {
        *out_priority = pq->heap[0].priority;
    }

    /* Move last element to root and sink down */
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size--;

    if (pq->size > 0) {
        heapify_down(pq, 0);
    }
    return 1;
}

/* Peek at root element: O(1) */
int pq_peek(const PriorityQueue *pq, int *out_song_id, int *out_priority) {
    if (pq_is_empty(pq)) {
        return 0;
    }
    if (out_song_id != NULL) {
        *out_song_id = pq->heap[0].song_id;
    }
    if (out_priority != NULL) {
        *out_priority = pq->heap[0].priority;
    }
    return 1;
}

/* Remove a song by ID (e.g. during cascade deletion) */
int pq_remove_song(PriorityQueue *pq, int song_id) {
    int removed_count = 0;
    for (int i = 0; i < pq->size; i++) {
        if (pq->heap[i].song_id == song_id) {
            pq->heap[i] = pq->heap[pq->size - 1];
            pq->size--;
            removed_count++;

            /* Fix heap property at index i */
            if (i < pq->size) {
                heapify_down(pq, i);
                heapify_up(pq, i);
            }
            i--; /* Re-check same index in case replacement also matches */
        }
    }
    return removed_count;
}

/* Display songs in priority queue sorted by ranking (non-destructive) */
void pq_display(const PriorityQueue *pq, const Library *lib) {
    printf("\n============================================================\n");
    printf("              PRIORITY PLAYBACK QUEUE (Max-Heap)            \n");
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-25s | %-18s | %-8s\n", "Rank", "ID", "Title", "Artist", "Priority");
    printf("------+------+---------------------------+--------------------+----------\n");

    if (pq_is_empty(pq)) {
        printf(" [Priority Playback Queue is currently empty]\n");
        printf("============================================================\n\n");
        return;
    }

    /* Make a copy of the priority queue to extract and display in ranked order */
    PriorityQueue copy;
    init_priority_queue(&copy, pq->size);
    for (int i = 0; i < pq->size; i++) {
        copy.heap[i] = pq->heap[i];
    }
    copy.size = pq->size;
    copy.counter = pq->counter;

    int rank = 1;
    int sid, pri;
    while (pq_extract_max(&copy, &sid, &pri)) {
        Song *s = find_song_by_id(lib, sid);
        if (s != NULL) {
            char short_title[26];
            char short_artist[19];
            strncpy(short_title, s->title, 25);
            short_title[25] = '\0';
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-25s | %-18s | Level %d\n",
                   rank, s->id, short_title, short_artist, pri);
        } else {
            printf(" %-4d | %-4d | [Song Not in Library]    | -                  | Level %d\n",
                   rank, sid, pri);
        }
        rank++;
    }

    free_priority_queue(&copy);
    printf("============================================================\n");
    printf("Total Priority Tracks: %d / %d\n\n", pq->size, pq->capacity);
}
