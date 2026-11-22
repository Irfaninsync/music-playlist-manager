#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <time.h>
#include "library.h"

#define DEFAULT_MAX_HISTORY 20

/* Node in the singly linked list for history */
typedef struct HistoryNode {
    int song_id;                /* Reference to song in library */
    time_t played_at;           /* Timestamp when the song was played */
    struct HistoryNode *next;   /* Pointer to the next older played song */
} HistoryNode;

/* Singly linked list representing listening history */
typedef struct HistoryList {
    HistoryNode *head;          /* Pointer to most recently played song */
    int count;                  /* Current number of history items */
    int max_capacity;           /* Maximum allowed history items (e.g. 20) */
} HistoryList;

/* Lifecycle and management */
void init_history(HistoryList *list, int max_capacity);
void free_history(HistoryList *list);
void clear_history(HistoryList *list);

/* Operations */
void add_to_history(HistoryList *list, int song_id);
int search_history(const HistoryList *list, int song_id);
int remove_song_from_history(HistoryList *list, int song_id);
void display_history(const HistoryList *list, const Library *lib);
int get_latest_played_song_id(const HistoryList *list);

#endif /* SINGLY_LINKED_LIST_H */
