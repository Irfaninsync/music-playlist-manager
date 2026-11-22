#ifndef LIBRARY_H
#define LIBRARY_H

#include "song.h"

#define INITIAL_CAPACITY 10

/* Dynamic Array structure for the main music library */
typedef struct Library {
    Song *songs;      /* Pointer to dynamically allocated array of Song records */
    int count;        /* Current number of songs in the library */
    int capacity;     /* Total allocated capacity of the array */
} Library;

/* Lifecycle functions */
void init_library(Library *lib, int initial_capacity);
void free_library(Library *lib);

/* Array CRUD operations */
int add_song(Library *lib, Song s);
int delete_song(Library *lib, int id);
int update_song(Library *lib, Song s);
Song* find_song_by_id(const Library *lib, int id);

/* Display functions */
void display_all_songs(const Library *lib);

/* Search algorithms */
int linear_search_by_title(const Library *lib, const char *title);
int linear_search_by_artist(const Library *lib, const char *artist);
int binary_search_by_id(const Library *lib, int id);
void filter_by_genre(const Library *lib, const char *genre);

/* Sorting algorithms */
void sort_songs_by_id(Library *lib);          /* Uses Bubble Sort */
void sort_songs_by_title(Library *lib);       /* Uses Selection Sort */
void sort_songs_by_artist(Library *lib);      /* Uses Insertion Sort */
void sort_songs_by_duration(Library *lib);    /* Uses Bubble Sort */

#endif /* LIBRARY_H */
