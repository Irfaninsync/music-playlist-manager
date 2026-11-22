#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/circular_linked_list.h"

/* Initialize the circular playlist */
void init_circular_playlist(CircularPlaylist *cpl) {
    cpl->head = NULL;
    cpl->current = NULL;
    cpl->count = 0;
    cpl->is_repeat_enabled = 1; /* Default to Repeat ON for this structure */
}

/* Clear all nodes */
void clear_circular_playlist(CircularPlaylist *cpl) {
    if (cpl->head == NULL) {
        cpl->current = NULL;
        cpl->count = 0;
        return;
    }

    CircularNode *curr = cpl->head;
    CircularNode *next_node = NULL;

    for (int i = 0; i < cpl->count; i++) {
        next_node = curr->next;
        free(curr);
        curr = next_node;
    }

    cpl->head = NULL;
    cpl->current = NULL;
    cpl->count = 0;
}

/* Free circular playlist */
void free_circular_playlist(CircularPlaylist *cpl) {
    clear_circular_playlist(cpl);
}

/* Add song to circular linked list (inserted at end of cycle) */
void circular_add_song(CircularPlaylist *cpl, int song_id) {
    CircularNode *new_node = (CircularNode *)malloc(sizeof(CircularNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for circular node!\n");
        return;
    }

    new_node->song_id = song_id;

    if (cpl->head == NULL) {
        /* First node points to itself */
        new_node->next = new_node;
        new_node->prev = new_node;
        cpl->head = new_node;
        cpl->current = new_node;
    } else {
        CircularNode *tail = cpl->head->prev;

        tail->next = new_node;
        new_node->prev = tail;
        new_node->next = cpl->head;
        cpl->head->prev = new_node;
    }
    cpl->count++;
}

/* Remove a song from the circular linked list by ID */
int circular_remove_song(CircularPlaylist *cpl, int song_id) {
    if (cpl->head == NULL || cpl->count == 0) {
        return 0;
    }

    CircularNode *curr = cpl->head;
    int removed_count = 0;
    int initial_count = cpl->count;

    for (int i = 0; i < initial_count; i++) {
        CircularNode *next_node = curr->next;

        if (curr->song_id == song_id) {
            if (cpl->count == 1) {
                /* Single node removal */
                free(curr);
                cpl->head = NULL;
                cpl->current = NULL;
                cpl->count = 0;
                return 1;
            }

            /* Adjust current cursor if pointing to deleted node */
            if (cpl->current == curr) {
                cpl->current = curr->next;
            }
            if (cpl->head == curr) {
                cpl->head = curr->next;
            }

            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;

            free(curr);
            cpl->count--;
            removed_count++;
        }
        curr = next_node;
    }

    return removed_count;
}

/* Toggle repeat mode */
void circular_toggle_repeat(CircularPlaylist *cpl) {
    cpl->is_repeat_enabled = !cpl->is_repeat_enabled;
    printf("\n[Repeat Mode is now: %s]\n", cpl->is_repeat_enabled ? "ENABLED (Looping)" : "DISABLED (Stop at end)");
}

/* Get current song in circular playlist */
Song* circular_get_current(const CircularPlaylist *cpl, const Library *lib) {
    if (cpl->current == NULL) {
        return NULL;
    }
    return find_song_by_id(lib, cpl->current->song_id);
}

/* Move to next song in circular playlist */
Song* circular_next(CircularPlaylist *cpl, const Library *lib) {
    if (cpl->head == NULL || cpl->current == NULL) {
        return NULL;
    }

    /* If repeat is OFF and we are at the last node (tail), stop */
    if (!cpl->is_repeat_enabled && cpl->current->next == cpl->head) {
        printf("\n[Notice] Repeat is OFF. Reached end of circular playlist.\n");
        return NULL;
    }

    cpl->current = cpl->current->next;
    return find_song_by_id(lib, cpl->current->song_id);
}

/* Move to previous song in circular playlist */
Song* circular_prev(CircularPlaylist *cpl, const Library *lib) {
    if (cpl->head == NULL || cpl->current == NULL) {
        return NULL;
    }

    /* If repeat is OFF and we are at the first node (head), stop */
    if (!cpl->is_repeat_enabled && cpl->current == cpl->head) {
        printf("\n[Notice] Repeat is OFF. At beginning of circular playlist.\n");
        return NULL;
    }

    cpl->current = cpl->current->prev;
    return find_song_by_id(lib, cpl->current->song_id);
}

/* Display the circular playlist using do-while traversal */
void display_circular_playlist(const CircularPlaylist *cpl, const Library *lib) {
    printf("\n============================================================\n");
    printf("        CIRCULAR REPEAT PLAYLIST (Repeat: %s)\n",
           cpl->is_repeat_enabled ? "ON" : "OFF");
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-25s | %-18s | %s\n", "No.", "ID", "Title", "Artist", "Status");
    printf("------+------+---------------------------+--------------------+--------\n");

    if (cpl->head == NULL || cpl->count == 0) {
        printf(" [Circular Playlist is empty]\n");
        printf("============================================================\n\n");
        return;
    }

    CircularNode *curr = cpl->head;
    int index = 1;

    /* Standard circular linked list do-while loop */
    do {
        Song *s = find_song_by_id(lib, curr->song_id);
        const char *cursor = (curr == cpl->current) ? "[>] PLAY" : " ";

        if (s != NULL) {
            char short_title[26];
            char short_artist[19];
            strncpy(short_title, s->title, 25);
            short_title[25] = '\0';
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-25s | %-18s | %s\n",
                   index, s->id, short_title, short_artist, cursor);
        } else {
            printf(" %-4d | %-4d | [Song Not in Library]    | -                  | %s\n",
                   index, curr->song_id, cursor);
        }

        curr = curr->next;
        index++;
    } while (curr != cpl->head);

    printf("============================================================\n");
    printf("Total Tracks in Cycle: %d (Loops seamlessly from tail -> head)\n\n", cpl->count);
}
