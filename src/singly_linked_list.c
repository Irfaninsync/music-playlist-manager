#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/singly_linked_list.h"

/* Initialize the history singly linked list */
void init_history(HistoryList *list, int max_capacity) {
    list->head = NULL;
    list->count = 0;
    list->max_capacity = (max_capacity > 0) ? max_capacity : DEFAULT_MAX_HISTORY;
}

/* Clear all nodes from history */
void clear_history(HistoryList *list) {
    HistoryNode *curr = list->head;
    while (curr != NULL) {
        HistoryNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->count = 0;
}

/* Free history list resources */
void free_history(HistoryList *list) {
    clear_history(list);
}

/* Add a song to history (Insert at Head so newest is first) */
void add_to_history(HistoryList *list, int song_id) {
    HistoryNode *new_node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for history node!\n");
        return;
    }

    new_node->song_id = song_id;
    new_node->played_at = time(NULL);
    new_node->next = list->head;
    list->head = new_node;
    list->count++;

    /* If history exceeds capacity, delete the oldest entry (last node) */
    if (list->count > list->max_capacity) {
        HistoryNode *curr = list->head;
        HistoryNode *prev = NULL;

        while (curr->next != NULL) {
            prev = curr;
            curr = curr->next;
        }

        /* curr is now the last node */
        if (prev != NULL) {
            prev->next = NULL;
            free(curr);
            list->count--;
        }
    }
}

/* Search if a song is in history */
int search_history(const HistoryList *list, int song_id) {
    HistoryNode *curr = list->head;
    int position = 1;

    while (curr != NULL) {
        if (curr->song_id == song_id) {
            return position; /* Found at 1-based index from top */
        }
        curr = curr->next;
        position++;
    }
    return 0; /* Not found */
}

/* Remove a specific song from history (used during cascade deletion) */
int remove_song_from_history(HistoryList *list, int song_id) {
    HistoryNode *curr = list->head;
    HistoryNode *prev = NULL;
    int removed_count = 0;

    while (curr != NULL) {
        if (curr->song_id == song_id) {
            HistoryNode *to_delete = curr;
            if (prev == NULL) {
                /* Deleting head node */
                list->head = curr->next;
                curr = list->head;
            } else {
                prev->next = curr->next;
                curr = curr->next;
            }
            free(to_delete);
            list->count--;
            removed_count++;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return removed_count;
}

/* Display the listening history from newest to oldest */
void display_history(const HistoryList *list, const Library *lib) {
    if (list->head == NULL || list->count == 0) {
        printf("\n[Recently Played History is currently empty.]\n");
        return;
    }

    printf("\n============================================================\n");
    printf("                  RECENTLY PLAYED HISTORY                   \n");
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-25s | %-18s | %-12s\n", "No.", "ID", "Title", "Artist", "Played Time");
    printf("------+------+---------------------------+--------------------+-------------\n");

    HistoryNode *curr = list->head;
    int index = 1;

    while (curr != NULL) {
        Song *s = find_song_by_id(lib, curr->song_id);

        char time_str[20];
        struct tm *tinfo = localtime(&curr->played_at);
        if (tinfo != NULL) {
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tinfo);
        } else {
            strcpy(time_str, "N/A");
        }

        if (s != NULL) {
            char short_title[26];
            char short_artist[19];
            strncpy(short_title, s->title, 25);
            short_title[25] = '\0';
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-25s | %-18s | %-12s\n",
                   index, s->id, short_title, short_artist, time_str);
        } else {
            printf(" %-4d | %-4d | [Song Not in Library]    | -                  | %-12s\n",
                   index, curr->song_id, time_str);
        }

        curr = curr->next;
        index++;
    }

    printf("============================================================\n");
    printf("Total History Entries: %d (Max limit: %d)\n\n", list->count, list->max_capacity);
}

/* Return the ID of the most recently played song */
int get_latest_played_song_id(const HistoryList *list) {
    if (list->head != NULL) {
        return list->head->song_id;
    }
    return -1;
}
