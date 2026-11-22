#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "library.h"

#define DEFAULT_HASH_TABLE_SIZE 31 /* Prime number for good hash distribution */

/* Collision chain node in hash table (Separate Chaining) */
typedef struct HashNode {
    int song_id;
    int library_index;          /* Index of song in Library array */
    struct HashNode *next;      /* Pointer to next collision node */
} HashNode;

/* Hash Table with Separate Chaining */
typedef struct HashTable {
    HashNode **buckets;         /* Array of linked list head pointers */
    int size;                   /* Number of buckets (e.g. 31) */
    int total_entries;          /* Total key-value pairs stored */
} HashTable;

/* Lifecycle functions */
void init_hash_table(HashTable *ht, int size);
void free_hash_table(HashTable *ht);
void clear_hash_table(HashTable *ht);

/* Core operations */
int hash_insert(HashTable *ht, int song_id, int library_index);
int hash_search(const HashTable *ht, int song_id);
int hash_delete(HashTable *ht, int song_id);
void rebuild_hash_table(HashTable *ht, const Library *lib);

/* Visual inspection */
void display_hash_table(const HashTable *ht, const Library *lib);

#endif /* HASH_TABLE_H */
