#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/file_handler.h"

/* Helper to strip trailing newline characters */
static void strip_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

/* Save all library songs to a CSV file */
int save_songs_to_file(const Library *lib, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("[Error] Could not open file '%s' for writing!\n", filename);
        return 0;
    }

    /* Write CSV header */
    fprintf(fp, "id,title,artist,album,genre,duration,file_path,is_favourite,priority\n");

    /* Write song rows */
    for (int i = 0; i < lib->count; i++) {
        Song *s = &lib->songs[i];
        fprintf(fp, "%d,%s,%s,%s,%s,%d,%s,%d,%d\n",
                s->id,
                s->title,
                s->artist,
                s->album,
                s->genre,
                s->duration,
                s->file_path,
                s->is_favourite,
                s->priority);
    }

    fclose(fp);
    return 1;
}

/* Load songs from CSV file into the library */
int load_songs_from_file(Library *lib, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[Notice] File '%s' not found. Starting with empty/default catalog.\n", filename);
        return 0;
    }

    char line[512];
    int loaded_count = 0;
    int is_header = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        strip_newline(line);
        if (strlen(line) == 0) {
            continue;
        }

        /* Skip header line */
        if (is_header) {
            is_header = 0;
            if (strncmp(line, "id,", 3) == 0 || strncmp(line, "ID,", 3) == 0) {
                continue;
            }
        }

        Song s;
        memset(&s, 0, sizeof(Song));

        char *token = strtok(line, ",");
        if (!token) continue;
        s.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.title, token, MAX_TITLE_LEN - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.artist, token, MAX_ARTIST_LEN - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.album, token, MAX_ALBUM_LEN - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.genre, token, MAX_GENRE_LEN - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.duration = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.file_path, token, MAX_PATH_LEN - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.is_favourite = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.priority = atoi(token);

        if (add_song(lib, s)) {
            loaded_count++;
        }
    }

    fclose(fp);
    return loaded_count;
}
