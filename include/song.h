#ifndef SONG_H
#define SONG_H

#include <stdio.h>

#define MAX_TITLE_LEN   80
#define MAX_ARTIST_LEN  60
#define MAX_ALBUM_LEN   60
#define MAX_GENRE_LEN   30
#define MAX_PATH_LEN    120

/* Structure representing a single song */
typedef struct Song {
    int id;                         /* Unique numeric ID (e.g., 1001, 1002) */
    char title[MAX_TITLE_LEN];      /* Song title */
    char artist[MAX_ARTIST_LEN];    /* Artist name */
    char album[MAX_ALBUM_LEN];      /* Album name */
    char genre[MAX_GENRE_LEN];      /* Musical genre */
    int duration;                   /* Duration in seconds */
    char file_path[MAX_PATH_LEN];   /* Path to audio file or simulated file */
    int is_favourite;               /* 1 = Favourite, 0 = Normal */
    int priority;                   /* Priority level: 1 (Lowest) to 10 (Highest) */
} Song;

/* Function prototypes for Song operations */
void format_duration(int total_seconds, char *buffer);
void print_song_row(const Song *s);
void print_song_card(const Song *s);

#endif /* SONG_H */
