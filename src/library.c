#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/library.h"

/* Case-insensitive string comparison helper */
static int case_insensitive_compare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = (char)tolower((unsigned char)*s1);
        char c2 = (char)tolower((unsigned char)*s2);
        if (c1 != c2) {
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/* Case-insensitive substring search helper */
static int contains_case_insensitive(const char *haystack, const char *needle) {
    if (!*needle) return 1;
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }
        if (!*n) return 1; /* Matched complete needle */
    }
    return 0;
}

/* Initialize dynamic array */
void init_library(Library *lib, int initial_capacity) {
    if (initial_capacity <= 0) {
        initial_capacity = INITIAL_CAPACITY;
    }
    lib->songs = (Song *)malloc(sizeof(Song) * initial_capacity);
    if (lib->songs == NULL) {
        printf("[Error] Memory allocation failed for Music Library!\n");
        lib->count = 0;
        lib->capacity = 0;
        return;
    }
    lib->count = 0;
    lib->capacity = initial_capacity;
}

/* Free allocated dynamic array */
void free_library(Library *lib) {
    if (lib->songs != NULL) {
        free(lib->songs);
        lib->songs = NULL;
    }
    lib->count = 0;
    lib->capacity = 0;
}

/* Add a new song to dynamic array, resizing if capacity is exceeded */
int add_song(Library *lib, Song s) {
    /* Check for duplicate ID */
    for (int i = 0; i < lib->count; i++) {
        if (lib->songs[i].id == s.id) {
            printf("[Warning] Song with ID %d already exists!\n", s.id);
            return 0;
        }
    }

    /* Resize if full */
    if (lib->count >= lib->capacity) {
        int new_capacity = (lib->capacity == 0) ? INITIAL_CAPACITY : lib->capacity * 2;
        Song *temp = (Song *)realloc(lib->songs, sizeof(Song) * new_capacity);
        if (temp == NULL) {
            printf("[Error] Failed to expand library memory!\n");
            return 0;
        }
        lib->songs = temp;
        lib->capacity = new_capacity;
    }

    lib->songs[lib->count] = s;
    lib->count++;
    return 1;
}

/* Delete a song by ID using array shifting */
int delete_song(Library *lib, int id) {
    int target_index = -1;
    for (int i = 0; i < lib->count; i++) {
        if (lib->songs[i].id == id) {
            target_index = i;
            break;
        }
    }

    if (target_index == -1) {
        return 0; /* Song not found */
    }

    /* Shift all elements after target_index to the left by 1 */
    for (int i = target_index; i < lib->count - 1; i++) {
        lib->songs[i] = lib->songs[i + 1];
    }
    lib->count--;
    return 1;
}

/* Update an existing song */
int update_song(Library *lib, Song s) {
    for (int i = 0; i < lib->count; i++) {
        if (lib->songs[i].id == s.id) {
            lib->songs[i] = s;
            return 1;
        }
    }
    return 0;
}

/* Search for a song pointer by ID */
Song* find_song_by_id(const Library *lib, int id) {
    for (int i = 0; i < lib->count; i++) {
        if (lib->songs[i].id == id) {
            return &lib->songs[i];
        }
    }
    return NULL;
}

/* Display all songs in tabular format */
void display_all_songs(const Library *lib) {
    if (lib->count == 0) {
        printf("\n[Music Library is empty. No songs to display.]\n");
        return;
    }

    printf("\n+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
    printf("| ID   | Title                     | Artist               | Genre      | Dur.  | Fav | Pri |\n");
    printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");

    for (int i = 0; i < lib->count; i++) {
        print_song_row(&lib->songs[i]);
    }

    printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
    printf("Total Songs: %d\n\n", lib->count);
}

/* Linear Search by Title (returns index or -1) */
int linear_search_by_title(const Library *lib, const char *title) {
    for (int i = 0; i < lib->count; i++) {
        if (contains_case_insensitive(lib->songs[i].title, title)) {
            return i;
        }
    }
    return -1;
}

/* Linear Search by Artist (returns index or -1) */
int linear_search_by_artist(const Library *lib, const char *artist) {
    for (int i = 0; i < lib->count; i++) {
        if (contains_case_insensitive(lib->songs[i].artist, artist)) {
            return i;
        }
    }
    return -1;
}

/* Filter and display songs by Genre */
void filter_by_genre(const Library *lib, const char *genre) {
    int matches = 0;
    printf("\n--- Filtered Songs for Genre: \"%s\" ---\n", genre);
    printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
    printf("| ID   | Title                     | Artist               | Genre      | Dur.  | Fav | Pri |\n");
    printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");

    for (int i = 0; i < lib->count; i++) {
        if (contains_case_insensitive(lib->songs[i].genre, genre)) {
            print_song_row(&lib->songs[i]);
            matches++;
        }
    }

    printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
    if (matches == 0) {
        printf("No songs found in genre: %s\n", genre);
    } else {
        printf("Found %d matching songs.\n", matches);
    }
}

/* Sort songs by ID using Bubble Sort */
void sort_songs_by_id(Library *lib) {
    for (int i = 0; i < lib->count - 1; i++) {
        for (int j = 0; j < lib->count - i - 1; j++) {
            if (lib->songs[j].id > lib->songs[j + 1].id) {
                Song temp = lib->songs[j];
                lib->songs[j] = lib->songs[j + 1];
                lib->songs[j + 1] = temp;
            }
        }
    }
}

/* Sort songs by Title using Selection Sort */
void sort_songs_by_title(Library *lib) {
    for (int i = 0; i < lib->count - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < lib->count; j++) {
            if (case_insensitive_compare(lib->songs[j].title, lib->songs[min_idx].title) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Song temp = lib->songs[i];
            lib->songs[i] = lib->songs[min_idx];
            lib->songs[min_idx] = temp;
        }
    }
}

/* Sort songs by Artist using Insertion Sort */
void sort_songs_by_artist(Library *lib) {
    for (int i = 1; i < lib->count; i++) {
        Song key = lib->songs[i];
        int j = i - 1;
        while (j >= 0 && case_insensitive_compare(lib->songs[j].artist, key.artist) > 0) {
            lib->songs[j + 1] = lib->songs[j];
            j--;
        }
        lib->songs[j + 1] = key;
    }
}

/* Sort songs by Duration using Bubble Sort */
void sort_songs_by_duration(Library *lib) {
    for (int i = 0; i < lib->count - 1; i++) {
        for (int j = 0; j < lib->count - i - 1; j++) {
            if (lib->songs[j].duration > lib->songs[j + 1].duration) {
                Song temp = lib->songs[j];
                lib->songs[j] = lib->songs[j + 1];
                lib->songs[j + 1] = temp;
            }
        }
    }
}

/* Binary Search by ID (Requires array to be sorted by ID) */
int binary_search_by_id(const Library *lib, int id) {
    int low = 0;
    int high = lib->count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (lib->songs[mid].id == id) {
            return mid; /* Found */
        }
        if (lib->songs[mid].id < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1; /* Not found */
}
