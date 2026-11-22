#include <stdio.h>
#include <string.h>
#include "../include/song.h"

/* Convert seconds into MM:SS format */
void format_duration(int total_seconds, char *buffer) {
    if (total_seconds < 0) {
        total_seconds = 0;
    }
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    sprintf(buffer, "%02d:%02d", minutes, seconds);
}

/* Print a single song as a clean row in a table */
void print_song_row(const Song *s) {
    char dur[16];
    format_duration(s->duration, dur);
    
    char fav_symbol = (s->is_favourite) ? '*' : ' ';

    /* Truncate strings visually if they are too long for the table column */
    char short_title[26];
    char short_artist[21];
    
    strncpy(short_title, s->title, 25);
    short_title[25] = '\0';

    strncpy(short_artist, s->artist, 20);
    short_artist[20] = '\0';

    printf("| %-4d | %-25s | %-20s | %-10s | %-5s |  %c  | %-3d |\n",
           s->id, short_title, short_artist, s->genre, dur, fav_symbol, s->priority);
}

/* Print a detailed card for a single song */
void print_song_card(const Song *s) {
    char dur[16];
    format_duration(s->duration, dur);

    printf("------------------------------------------------------------\n");
    printf(" Song ID:      %d\n", s->id);
    printf(" Title:        %s\n", s->title);
    printf(" Artist:       %s\n", s->artist);
    printf(" Album:        %s\n", s->album);
    printf(" Genre:        %s\n", s->genre);
    printf(" Duration:     %s (%d seconds)\n", dur, s->duration);
    printf(" Favourite:    %s\n", s->is_favourite ? "Yes (*)" : "No");
    printf(" Priority:     %d / 10\n", s->priority);
    printf(" File Path:    %s\n", s->file_path);
    printf("------------------------------------------------------------\n");
}
