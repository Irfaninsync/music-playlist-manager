#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/doubly_linked_list.h"

/* Initialize a new empty playlist */
void init_playlist(Playlist *pl, const char *name) {
    if (name != NULL && strlen(name) > 0) {
        strncpy(pl->name, name, MAX_PLAYLIST_NAME - 1);
        pl->name[MAX_PLAYLIST_NAME - 1] = '\0';
    } else {
        strcpy(pl->name, "My Playlist");
    }
    pl->head = NULL;
    pl->tail = NULL;
    pl->current = NULL;
    pl->count = 0;
}

/* Clear all nodes from the playlist */
void clear_playlist(Playlist *pl) {
    PlaylistNode *curr = pl->head;
    while (curr != NULL) {
        PlaylistNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    pl->head = NULL;
    pl->tail = NULL;
    pl->current = NULL;
    pl->count = 0;
}

/* Free playlist resources */
void free_playlist(Playlist *pl) {
    clear_playlist(pl);
}

/* Add song at the end of the playlist (Append) */
void add_to_playlist_end(Playlist *pl, int song_id) {
    PlaylistNode *new_node = (PlaylistNode *)malloc(sizeof(PlaylistNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for playlist node!\n");
        return;
    }

    new_node->song_id = song_id;
    new_node->next = NULL;
    new_node->prev = pl->tail;

    if (pl->head == NULL) {
        /* Empty playlist: new node is both head and tail */
        pl->head = new_node;
        pl->tail = new_node;
        pl->current = new_node;
    } else {
        pl->tail->next = new_node;
        pl->tail = new_node;
    }
    pl->count++;
}

/* Add song at the beginning of the playlist (Prepend) */
void add_to_playlist_start(Playlist *pl, int song_id) {
    PlaylistNode *new_node = (PlaylistNode *)malloc(sizeof(PlaylistNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for playlist node!\n");
        return;
    }

    new_node->song_id = song_id;
    new_node->prev = NULL;
    new_node->next = pl->head;

    if (pl->head == NULL) {
        pl->head = new_node;
        pl->tail = new_node;
        pl->current = new_node;
    } else {
        pl->head->prev = new_node;
        pl->head = new_node;
    }
    pl->count++;
}

/* Insert a song at a specific 1-based position */
int insert_into_playlist_at(Playlist *pl, int song_id, int position) {
    if (position < 1 || position > pl->count + 1) {
        printf("[Error] Invalid position %d. Allowed range is 1 to %d.\n", position, pl->count + 1);
        return 0;
    }

    if (position == 1) {
        add_to_playlist_start(pl, song_id);
        return 1;
    }
    if (position == pl->count + 1) {
        add_to_playlist_end(pl, song_id);
        return 1;
    }

    /* Traverse to the node currently occupying the target position */
    PlaylistNode *curr = pl->head;
    for (int i = 1; i < position; i++) {
        curr = curr->next;
    }

    PlaylistNode *new_node = (PlaylistNode *)malloc(sizeof(PlaylistNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for playlist node!\n");
        return 0;
    }

    new_node->song_id = song_id;
    new_node->prev = curr->prev;
    new_node->next = curr;

    curr->prev->next = new_node;
    curr->prev = new_node;

    pl->count++;
    return 1;
}

/* Remove track at 1-based position and store removed song ID for undo */
int remove_from_playlist_at(Playlist *pl, int position, int *removed_id) {
    if (position < 1 || position > pl->count || pl->head == NULL) {
        return 0;
    }

    PlaylistNode *curr = pl->head;
    for (int i = 1; i < position; i++) {
        curr = curr->next;
    }

    if (removed_id != NULL) {
        *removed_id = curr->song_id;
    }

    /* Update current playback cursor if it was pointing to this node */
    if (pl->current == curr) {
        pl->current = (curr->next != NULL) ? curr->next : curr->prev;
    }

    /* Rewire adjacent node pointers */
    if (curr == pl->head && curr == pl->tail) {
        /* Single node in list */
        pl->head = NULL;
        pl->tail = NULL;
    } else if (curr == pl->head) {
        /* Removing head */
        pl->head = curr->next;
        pl->head->prev = NULL;
    } else if (curr == pl->tail) {
        /* Removing tail */
        pl->tail = curr->prev;
        pl->tail->next = NULL;
    } else {
        /* Removing middle node */
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
    }

    free(curr);
    pl->count--;
    return 1;
}

/* Remove all occurrences of a song by ID (e.g. on library song deletion) */
int remove_song_from_playlist(Playlist *pl, int song_id) {
    PlaylistNode *curr = pl->head;
    int pos = 1;
    int removed_count = 0;

    while (curr != NULL) {
        PlaylistNode *next_node = curr->next;
        if (curr->song_id == song_id) {
            remove_from_playlist_at(pl, pos, NULL);
            removed_count++;
            /* Since we removed current node, pos remains the same for next check */
        } else {
            pos++;
        }
        curr = next_node;
    }
    return removed_count;
}

/* Move song from one position to another in playlist */
int move_song_in_playlist(Playlist *pl, int from_pos, int to_pos) {
    if (from_pos < 1 || from_pos > pl->count || to_pos < 1 || to_pos > pl->count) {
        printf("[Error] Invalid position range for moving songs.\n");
        return 0;
    }
    if (from_pos == to_pos) {
        return 1; /* Already at desired position */
    }

    int song_id;
    if (!remove_from_playlist_at(pl, from_pos, &song_id)) {
        return 0;
    }

    return insert_into_playlist_at(pl, song_id, to_pos);
}

/* Get currently pointed song */
Song* playlist_get_current(const Playlist *pl, const Library *lib) {
    if (pl->current == NULL) {
        return NULL;
    }
    return find_song_by_id(lib, pl->current->song_id);
}

/* Move to next song in playlist */
Song* playlist_next(Playlist *pl, const Library *lib) {
    if (pl->head == NULL) {
        return NULL;
    }
    if (pl->current == NULL) {
        pl->current = pl->head;
    } else if (pl->current->next != NULL) {
        pl->current = pl->current->next;
    } else {
        /* Reached end of playlist without repeat mode */
        printf("\n[Notice] Reached the end of the playlist '%s'.\n", pl->name);
        return NULL;
    }
    return find_song_by_id(lib, pl->current->song_id);
}

/* Move to previous song in playlist */
Song* playlist_prev(Playlist *pl, const Library *lib) {
    if (pl->head == NULL) {
        return NULL;
    }
    if (pl->current == NULL) {
        pl->current = pl->head;
    } else if (pl->current->prev != NULL) {
        pl->current = pl->current->prev;
    } else {
        printf("\n[Notice] Already at the first song of playlist '%s'.\n", pl->name);
        return NULL;
    }
    return find_song_by_id(lib, pl->current->song_id);
}

/* Set current cursor by 1-based position */
void playlist_set_current_by_position(Playlist *pl, int position) {
    if (position < 1 || position > pl->count) {
        return;
    }
    PlaylistNode *curr = pl->head;
    for (int i = 1; i < position; i++) {
        curr = curr->next;
    }
    pl->current = curr;
}

/* Display complete playlist */
void display_playlist(const Playlist *pl, const Library *lib) {
    printf("\n============================================================\n");
    printf(" PLAYLIST: %-48s\n", pl->name);
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-25s | %-18s | %-6s | %s\n",
           "No.", "ID", "Title", "Artist", "Dur.", "Status");
    printf("------+------+---------------------------+--------------------+--------+--------\n");

    if (pl->head == NULL || pl->count == 0) {
        printf(" [Playlist is currently empty]\n");
        printf("============================================================\n\n");
        return;
    }

    PlaylistNode *curr = pl->head;
    int index = 1;

    while (curr != NULL) {
        Song *s = find_song_by_id(lib, curr->song_id);
        const char *cursor = (curr == pl->current) ? "[>] PLAY" : " ";

        if (s != NULL) {
            char dur[16];
            format_duration(s->duration, dur);

            char short_title[26];
            char short_artist[19];
            strncpy(short_title, s->title, 25);
            short_title[25] = '\0';
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-25s | %-18s | %-6s | %s\n",
                   index, s->id, short_title, short_artist, dur, cursor);
        } else {
            printf(" %-4d | %-4d | [Song Not in Library]    | -                  | -      | %s\n",
                   index, curr->song_id, cursor);
        }

        curr = curr->next;
        index++;
    }

    printf("============================================================\n");
    printf("Total Tracks: %d\n\n", pl->count);
}
