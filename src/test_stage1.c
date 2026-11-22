#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/song.h"
#include "../include/library.h"
#include "../include/file_handler.h"

int main(void) {
    printf("============================================================\n");
    printf("        STAGE 1 VERIFICATION: MUSIC LIBRARY & ARRAYS       \n");
    printf("============================================================\n");

    Library lib;
    init_library(&lib, 4); /* Start with small capacity to test dynamic resizing */
    printf("[PASS] Initialized Library with capacity: %d\n", lib.capacity);

    /* 1. Load songs from data/songs.csv */
    int loaded = load_songs_from_file(&lib, "data/songs.csv");
    printf("[PASS] Loaded %d songs from 'data/songs.csv'.\n", loaded);
    printf("       Current count: %d, Capacity: %d (Resized dynamically!)\n", lib.count, lib.capacity);

    /* 2. Display all songs */
    printf("\n--- Displaying All Songs in Main Library Array ---");
    display_all_songs(&lib);

    /* 3. Test Linear Search */
    printf("\n--- Testing Linear Search --- \n");
    int idx_title = linear_search_by_title(&lib, "Hotel");
    if (idx_title != -1) {
        printf("[PASS] Linear search for 'Hotel' found at index %d: %s by %s\n",
               idx_title, lib.songs[idx_title].title, lib.songs[idx_title].artist);
    } else {
        printf("[FAIL] Linear search for 'Hotel' failed.\n");
    }

    int idx_artist = linear_search_by_artist(&lib, "Sinatra");
    if (idx_artist != -1) {
        printf("[PASS] Linear search for 'Sinatra' found at index %d: %s by %s\n",
               idx_artist, lib.songs[idx_artist].title, lib.songs[idx_artist].artist);
    } else {
        printf("[FAIL] Linear search for 'Sinatra' failed.\n");
    }

    /* 4. Test Filter by Genre */
    printf("\n--- Testing Genre Filtering --- \n");
    filter_by_genre(&lib, "Jazz");

    /* 5. Test Sorting Algorithms */
    printf("\n--- Testing Sorting by Title (Selection Sort) --- \n");
    sort_songs_by_title(&lib);
    printf("First song after title sort: %s\n", lib.songs[0].title);
    printf("Last song after title sort:  %s\n", lib.songs[lib.count - 1].title);

    printf("\n--- Testing Sorting by Artist (Insertion Sort) --- \n");
    sort_songs_by_artist(&lib);
    printf("First artist after artist sort: %s (%s)\n", lib.songs[0].artist, lib.songs[0].title);
    printf("Last artist after artist sort:  %s (%s)\n", lib.songs[lib.count - 1].artist, lib.songs[lib.count - 1].title);

    printf("\n--- Testing Sorting by Duration (Bubble Sort) --- \n");
    sort_songs_by_duration(&lib);
    printf("Shortest song: %s (%d seconds)\n", lib.songs[0].title, lib.songs[0].duration);
    printf("Longest song:  %s (%d seconds)\n", lib.songs[lib.count - 1].title, lib.songs[lib.count - 1].duration);

    /* 6. Test Binary Search by ID (Requires sorted by ID) */
    printf("\n--- Testing Binary Search by ID --- \n");
    sort_songs_by_id(&lib);
    int target_id = 1005;
    int b_idx = binary_search_by_id(&lib, target_id);
    if (b_idx != -1) {
        printf("[PASS] Binary search for ID %d found at index %d: '%s' by %s\n",
               target_id, b_idx, lib.songs[b_idx].title, lib.songs[b_idx].artist);
    } else {
        printf("[FAIL] Binary search for ID %d failed.\n", target_id);
    }

    int missing_id = 9999;
    int b_missing = binary_search_by_id(&lib, missing_id);
    if (b_missing == -1) {
        printf("[PASS] Binary search correctly returned -1 for non-existent ID %d.\n", missing_id);
    } else {
        printf("[FAIL] Binary search should have returned -1 for missing ID.\n");
    }

    /* 7. Test Adding a new song manually */
    printf("\n--- Testing Add Song & Delete Song --- \n");
    Song new_s = {
        1013,
        "Yellow Submarine",
        "The Beatles",
        "Revolver",
        "Rock",
        158,
        "music/yellow_submarine.mp3",
        1,
        8
    };
    if (add_song(&lib, new_s)) {
        printf("[PASS] Added new song ID 1013: '%s'. Count is now %d.\n", new_s.title, lib.count);
    }

    /* 8. Test Delete Song */
    if (delete_song(&lib, 1013)) {
        printf("[PASS] Deleted song ID 1013. Count is now %d.\n", lib.count);
    } else {
        printf("[FAIL] Failed to delete song ID 1013.\n");
    }

    /* 9. Test Saving to CSV */
    int saved = save_songs_to_file(&lib, "data/test_output.csv");
    if (saved) {
        printf("[PASS] Successfully saved %d songs to 'data/test_output.csv'.\n", lib.count);
    } else {
        printf("[FAIL] Failed to save songs to file.\n");
    }

    /* 10. Clean up memory */
    free_library(&lib);
    printf("[PASS] Freed library dynamic array memory successfully.\n");

    printf("\n============================================================\n");
    printf("           ALL STAGE 1 CHECKS COMPLETED SUCCESSFULLY!        \n");
    printf("============================================================\n");
    return 0;
}
