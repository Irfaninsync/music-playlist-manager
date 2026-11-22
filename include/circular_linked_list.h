#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H

#include "library.h"

/* Node for the circular linked list */
typedef struct CircularNode {
    int song_id;
    struct CircularNode *next;
    struct CircularNode *prev;
} CircularNode;

/* Circular Doubly Linked List for Repeat Playback Mode */
typedef struct CircularPlaylist {
    CircularNode *head;
    CircularNode *current;
    int count;
    int is_repeat_enabled;      /* 1 = Repeat Mode ON, 0 = Repeat Mode OFF */
} CircularPlaylist;

/* Lifecycle functions */
void init_circular_playlist(CircularPlaylist *cpl);
void free_circular_playlist(CircularPlaylist *cpl);
void clear_circular_playlist(CircularPlaylist *cpl);

/* Operations */
void circular_add_song(CircularPlaylist *cpl, int song_id);
int circular_remove_song(CircularPlaylist *cpl, int song_id);
void circular_toggle_repeat(CircularPlaylist *cpl);
void display_circular_playlist(const CircularPlaylist *cpl, const Library *lib);

/* Navigation */
Song* circular_next(CircularPlaylist *cpl, const Library *lib);
Song* circular_prev(CircularPlaylist *cpl, const Library *lib);
Song* circular_get_current(const CircularPlaylist *cpl, const Library *lib);

#endif /* CIRCULAR_LINKED_LIST_H */
