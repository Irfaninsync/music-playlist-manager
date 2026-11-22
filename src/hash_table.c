#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_table.h"

/* Modulo Hash Function for integer Song IDs */
static int hash_function(int key, int table_size) {
    if (key < 0) key = -key;
    return key % table_size;
}

/* Initialize the hash table */
void init_hash_table(HashTable *ht, int size) {
    if (size <= 0) {
        size = DEFAULT_HASH_TABLE_SIZE;
    }
    ht->size = size;
    ht->total_entries = 0;
    ht->buckets = (HashNode **)malloc(sizeof(HashNode *) * size);
    if (ht->buckets == NULL) {
        printf("[Error] Memory allocation failed for Hash Table buckets!\n");
        ht->size = 0;
        return;
    }

    for (int i = 0; i < size; i++) {
        ht->buckets[i] = NULL;
    }
}

/* Clear all entries from hash table */
void clear_hash_table(HashTable *ht) {
    if (ht->buckets == NULL) return;

    for (int i = 0; i < ht->size; i++) {
        HashNode *curr = ht->buckets[i];
        while (curr != NULL) {
            HashNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        ht->buckets[i] = NULL;
    }
    ht->total_entries = 0;
}

/* Free hash table */
void free_hash_table(HashTable *ht) {
    if (ht->buckets != NULL) {
        clear_hash_table(ht);
        free(ht->buckets);
        ht->buckets = NULL;
    }
    ht->size = 0;
    ht->total_entries = 0;
}

/* Insert a song ID and its library index into the hash table */
int hash_insert(HashTable *ht, int song_id, int library_index) {
    if (ht->buckets == NULL || ht->size == 0) {
        return 0;
    }

    int bucket = hash_function(song_id, ht->size);

    /* Check if song_id already exists in the collision chain; update if found */
    HashNode *curr = ht->buckets[bucket];
    while (curr != NULL) {
        if (curr->song_id == song_id) {
            curr->library_index = library_index;
            return 1;
        }
        curr = curr->next;
    }

    /* Key not found: insert at the head of the chain (Separate Chaining) */
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        printf("[Error] Memory allocation failed for HashNode!\n");
        return 0;
    }

    new_node->song_id = song_id;
    new_node->library_index = library_index;
    new_node->next = ht->buckets[bucket];
    ht->buckets[bucket] = new_node;
    ht->total_entries++;

    return 1;
}

/* Search for a song by ID: O(1) average time */
int hash_search(const HashTable *ht, int song_id) {
    if (ht->buckets == NULL || ht->size == 0) {
        return -1;
    }

    int bucket = hash_function(song_id, ht->size);
    HashNode *curr = ht->buckets[bucket];

    while (curr != NULL) {
        if (curr->song_id == song_id) {
            return curr->library_index; /* Found */
        }
        curr = curr->next;
    }

    return -1; /* Not found */
}

/* Delete a song ID from the hash table */
int hash_delete(HashTable *ht, int song_id) {
    if (ht->buckets == NULL || ht->size == 0) {
        return 0;
    }

    int bucket = hash_function(song_id, ht->size);
    HashNode *curr = ht->buckets[bucket];
    HashNode *prev = NULL;

    while (curr != NULL) {
        if (curr->song_id == song_id) {
            if (prev == NULL) {
                ht->buckets[bucket] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            ht->total_entries--;
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }

    return 0; /* Not found */
}

/* Rebuild hash table mapping directly from Library array */
void rebuild_hash_table(HashTable *ht, const Library *lib) {
    clear_hash_table(ht);
    for (int i = 0; i < lib->count; i++) {
        hash_insert(ht, lib->songs[i].id, i);
    }
}

/* Display the internal structure of the hash table (Buckets & Chains) */
void display_hash_table(const HashTable *ht, const Library *lib) {
    printf("\n============================================================\n");
    printf("         HASH TABLE INSPECTION (Separate Chaining)          \n");
    printf("============================================================\n");
    printf("Total Buckets: %d | Total Entries: %d | Load Factor: %.2f\n",
           ht->size, ht->total_entries, (float)ht->total_entries / ht->size);
    printf("------------------------------------------------------------\n");

    int active_buckets = 0;
    int collisions = 0;

    for (int i = 0; i < ht->size; i++) {
        if (ht->buckets[i] != NULL) {
            active_buckets++;
            printf(" Bucket [%02d] -> ", i);

            HashNode *curr = ht->buckets[i];
            int chain_len = 0;

            while (curr != NULL) {
                Song *s = find_song_by_id(lib, curr->song_id);
                if (s != NULL) {
                    printf("[ID:%d \"%s\"]", curr->song_id, s->title);
                } else {
                    printf("[ID:%d]", curr->song_id);
                }

                if (curr->next != NULL) {
                    printf(" -> ");
                    collisions++;
                }
                chain_len++;
                curr = curr->next;
            }
            printf(" (Len: %d)\n", chain_len);
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Active Buckets: %d / %d | Collisions Handled: %d\n", active_buckets, ht->size, collisions);
    printf("============================================================\n\n");
}
