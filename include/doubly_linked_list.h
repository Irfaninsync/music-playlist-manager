#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "library.h"

#define MAX_PLAYLIST_NAME 60

/* Node in the doubly linked list representing a track in the playlist */
typedef struct PlaylistNode {
    int song_id;                    /* ID of the song in the library */
    struct PlaylistNode *prev;      /* Pointer to previous song node */
    struct PlaylistNode *next;      /* Pointer to next song node */
} PlaylistNode;

/* Doubly linked list representing an active music playlist */
typedef struct Playlist {
    char name[MAX_PLAYLIST_NAME];   /* Name of the playlist */
    PlaylistNode *head;             /* Pointer to the first track */
    PlaylistNode *tail;             /* Pointer to the last track */
    PlaylistNode *current;          /* Playback cursor (currently playing track) */
    int count;                      /* Total songs in playlist */
} Playlist;

/* Lifecycle functions */
void init_playlist(Playlist *pl, const char *name);
void clear_playlist(Playlist *pl);
void free_playlist(Playlist *pl);

/* Insertion operations */
void add_to_playlist_end(Playlist *pl, int song_id);
void add_to_playlist_start(Playlist *pl, int song_id);
int insert_into_playlist_at(Playlist *pl, int song_id, int position);

/* Deletion operations */
int remove_from_playlist_at(Playlist *pl, int position, int *removed_id);
int remove_song_from_playlist(Playlist *pl, int song_id);

/* Reordering */
int move_song_in_playlist(Playlist *pl, int from_pos, int to_pos);

/* Playback navigation */
Song* playlist_get_current(const Playlist *pl, const Library *lib);
Song* playlist_next(Playlist *pl, const Library *lib);
Song* playlist_prev(Playlist *pl, const Library *lib);
void playlist_set_current_by_position(Playlist *pl, int position);

/* Display */
void display_playlist(const Playlist *pl, const Library *lib);

#endif /* DOUBLY_LINKED_LIST_H */
