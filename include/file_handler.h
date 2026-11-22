#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "library.h"

/* Save all library songs to CSV file */
int save_songs_to_file(const Library *lib, const char *filename);

/* Load songs from CSV file into the library */
int load_songs_from_file(Library *lib, const char *filename);

#endif /* FILE_HANDLER_H */
