#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/interface.h"

/* Safe string input reading from stdin */
void safe_read_string(const char *prompt, char *buffer, int max_len) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
    } else {
        buffer[0] = '\0';
    }
}

/* Safe integer input reading with range validation */
int safe_read_int(const char *prompt, int min_val, int max_val) {
    char line[64];
    int val = 0;
    while (1) {
        if (prompt != NULL) {
            printf("%s", prompt);
        }
        if (fgets(line, sizeof(line), stdin) != NULL) {
            char *endptr;
            val = (int)strtol(line, &endptr, 10);
            if (endptr != line) {
                if (val >= min_val && val <= max_val) {
                    return val;
                }
            }
        }
        printf(" [!] Invalid input. Please enter a number between %d and %d: ", min_val, max_val);
    }
}

/* Synchronize auxiliary data structures (Hash Table, BST, Graph, Circular Repeat) */
void sync_all_indices(AppContext *ctx) {
    rebuild_hash_table(&ctx->hash_table, &ctx->library);
    rebuild_bst(&ctx->bst_tree, &ctx->library);
    build_recommendation_graph(&ctx->recommendation_graph, &ctx->library);

    /* Sync active playlist with repeat circular playlist */
    clear_circular_playlist(&ctx->repeat_playlist);
    if (ctx->playlist_count > 0 && ctx->active_playlist_idx >= 0) {
        Playlist *pl = &ctx->playlists[ctx->active_playlist_idx];
        PlaylistNode *curr = pl->head;
        while (curr != NULL) {
            circular_add_song(&ctx->repeat_playlist, curr->song_id);
            curr = curr->next;
        }
    }
}

/* Initialize all application subsystems */
void init_app_context(AppContext *ctx) {
    init_library(&ctx->library, 20);
    init_history(&ctx->history, 20);
    init_stack(&ctx->undo_stack, 30);
    init_queue(&ctx->upcoming_queue, 20);
    init_priority_queue(&ctx->priority_queue, 20);
    init_hash_table(&ctx->hash_table, 31);
    init_bst(&ctx->bst_tree);
    init_graph(&ctx->recommendation_graph, 30);
    init_circular_playlist(&ctx->repeat_playlist);

    ctx->playlist_count = 0;
    ctx->active_playlist_idx = 0;
    ctx->is_playing = 0;
    ctx->current_song_id = 0;

    /* Initialize default playlist */
    init_playlist(&ctx->playlists[0], "Favorites Mix");
    ctx->playlist_count = 1;
}

/* Free all application dynamically allocated structures */
void free_app_context(AppContext *ctx) {
    free_library(&ctx->library);
    free_history(&ctx->history);
    free_stack(&ctx->undo_stack);
    free_queue(&ctx->upcoming_queue);
    free_priority_queue(&ctx->priority_queue);
    free_hash_table(&ctx->hash_table);
    free_bst(&ctx->bst_tree);
    free_graph(&ctx->recommendation_graph);
    free_circular_playlist(&ctx->repeat_playlist);

    for (int i = 0; i < ctx->playlist_count; i++) {
        free_playlist(&ctx->playlists[i]);
    }
}

/* Save application data to disk */
void save_all_data(AppContext *ctx) {
    save_songs_to_file(&ctx->library, "data/songs.csv");

    /* Save playlists */
    FILE *fp = fopen("data/playlists.csv", "w");
    if (fp != NULL) {
        for (int i = 0; i < ctx->playlist_count; i++) {
            Playlist *pl = &ctx->playlists[i];
            fprintf(fp, "%s", pl->name);
            PlaylistNode *curr = pl->head;
            while (curr != NULL) {
                fprintf(fp, ",%d", curr->song_id);
                curr = curr->next;
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }

    /* Save history */
    fp = fopen("data/history.csv", "w");
    if (fp != NULL) {
        HistoryNode *curr = ctx->history.head;
        while (curr != NULL) {
            fprintf(fp, "%d,%ld\n", curr->song_id, (long)curr->played_at);
            curr = curr->next;
        }
        fclose(fp);
    }

    printf("\n[Success] All application data saved to disk in 'data/' directory.\n");
}

/* Load application data from disk */
void load_all_data(AppContext *ctx) {
    load_songs_from_file(&ctx->library, "data/songs.csv");

    /* Load playlists */
    FILE *fp = fopen("data/playlists.csv", "r");
    if (fp != NULL) {
        char line[512];
        int loaded_playlists = 0;

        while (fgets(line, sizeof(line), fp) != NULL && loaded_playlists < MAX_PLAYLISTS) {
            size_t len = strlen(line);
            while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
                line[len - 1] = '\0';
                len--;
            }
            if (len == 0) continue;

            char *token = strtok(line, ",");
            if (token != NULL) {
                if (loaded_playlists == 0) {
                    clear_playlist(&ctx->playlists[0]);
                    strncpy(ctx->playlists[0].name, token, MAX_PLAYLIST_NAME - 1);
                } else {
                    init_playlist(&ctx->playlists[loaded_playlists], token);
                }

                while ((token = strtok(NULL, ",")) != NULL) {
                    int sid = atoi(token);
                    if (find_song_by_id(&ctx->library, sid) != NULL) {
                        add_to_playlist_end(&ctx->playlists[loaded_playlists], sid);
                    }
                }
                loaded_playlists++;
            }
        }
        fclose(fp);
        if (loaded_playlists > 0) {
            ctx->playlist_count = loaded_playlists;
        }
    } else {
        /* If no playlist file, add first 4 songs to default playlist */
        for (int i = 0; i < ctx->library.count && i < 4; i++) {
            add_to_playlist_end(&ctx->playlists[0], ctx->library.songs[i].id);
        }
    }

    /* Load history */
    fp = fopen("data/history.csv", "r");
    if (fp != NULL) {
        char line[128];
        while (fgets(line, sizeof(line), fp) != NULL) {
            int sid = 0;
            long t = 0;
            if (sscanf(line, "%d,%ld", &sid, &t) >= 1) {
                if (find_song_by_id(&ctx->library, sid) != NULL) {
                    add_to_history(&ctx->history, sid);
                }
            }
        }
        fclose(fp);
    }

    /* Pre-populate priority queue with songs marked with priority >= 8 */
    for (int i = 0; i < ctx->library.count; i++) {
        if (ctx->library.songs[i].priority >= 8) {
            pq_insert(&ctx->priority_queue, ctx->library.songs[i].id, ctx->library.songs[i].priority);
        }
    }

    sync_all_indices(ctx);
}

/* Display Main Menu */
void print_main_menu(void) {
    printf("\n============================================================\n");
    printf("                  MUSIC PLAYLIST MANAGER                    \n");
    printf("          Data Structures & Algorithms Project (C11)        \n");
    printf("============================================================\n");
    printf("  [1]  Music Library          (View, Add, Edit, Delete, Filter)\n");
    printf("  [2]  Playlist Management    (Create, Edit, Order, Tracks)\n");
    printf("  [3]  Music Player           (Play, Pause, Next, Prev, Repeat)\n");
    printf("  [4]  Upcoming Queue         (FIFO Playback Staging)\n");
    printf("  [5]  Recently Played        (Listening History Log)\n");
    printf("  [6]  Favourites & Priority  (VIP Songs & Max-Heap Playback)\n");
    printf("  [7]  Search and Sort Songs  (Linear, Binary Search, Multi-Sort)\n");
    printf("  [8]  Song Recommendations   (Graph BFS / DFS Explorer)\n");
    printf("  [9]  Undo Last Action       (Stack Reversal Engine)\n");
    printf("  [10] DSA Demonstration      (Interactive DS Visualizer)\n");
    printf("  [11] Save Catalog & Exit\n");
    printf("============================================================\n");
}

/* Cascade song deletion across all auxiliary structures */
static void cascade_delete_song(AppContext *ctx, int song_id, const char *title) {
    /* 1. Delete from main library array */
    delete_song(&ctx->library, song_id);

    /* 2. Remove from all playlists */
    for (int i = 0; i < ctx->playlist_count; i++) {
        remove_song_from_playlist(&ctx->playlists[i], song_id);
    }

    /* 3. Remove from history */
    remove_song_from_history(&ctx->history, song_id);

    /* 4. Remove from repeat circular list */
    circular_remove_song(&ctx->repeat_playlist, song_id);

    /* 5. Remove from upcoming queue */
    remove_song_from_queue(&ctx->upcoming_queue, song_id);

    /* 6. Remove from priority queue */
    pq_remove_song(&ctx->priority_queue, song_id);

    /* 7. Remove from hash table */
    hash_delete(&ctx->hash_table, song_id);

    /* 8. Remove from BST */
    if (title != NULL && strlen(title) > 0) {
        bst_delete(&ctx->bst_tree, title);
    }

    /* 9. Remove from graph */
    graph_remove_vertex(&ctx->recommendation_graph, song_id);

    /* 10. If currently playing this song, reset */
    if (ctx->current_song_id == song_id) {
        ctx->current_song_id = 0;
        ctx->is_playing = 0;
    }

    sync_all_indices(ctx);
    printf("[Success] Song ID %d deleted and pruned across all playlists and queues.\n", song_id);
}

/* 1. Music Library Submenu */
void handle_music_library_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Music Library Management ---\n");
        printf(" 1. View All Songs (Array Traversal)\n");
        printf(" 2. Add New Song\n");
        printf(" 3. Edit Song Details\n");
        printf(" 4. Delete Song (Cascade Removal)\n");
        printf(" 5. Search by ID (Hash Table Lookup)\n");
        printf(" 6. Search by Title (Linear Search)\n");
        printf(" 7. Search by Artist (Linear Search)\n");
        printf(" 8. Filter by Genre\n");
        printf(" 9. Display Total Songs Count\n");
        printf(" 10. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-10]: ", 1, 10);
        if (choice == 10) break;

        switch (choice) {
            case 1:
                display_all_songs(&ctx->library);
                break;
            case 2: {
                Song s;
                memset(&s, 0, sizeof(Song));
                s.id = safe_read_int("Enter new Song ID (e.g. 1020): ", 1, 99999);
                if (find_song_by_id(&ctx->library, s.id) != NULL) {
                    printf("[Error] Song with ID %d already exists!\n", s.id);
                    break;
                }
                safe_read_string("Enter Song Title: ", s.title, MAX_TITLE_LEN);
                safe_read_string("Enter Artist Name: ", s.artist, MAX_ARTIST_LEN);
                safe_read_string("Enter Album Name: ", s.album, MAX_ALBUM_LEN);
                safe_read_string("Enter Genre: ", s.genre, MAX_GENRE_LEN);
                s.duration = safe_read_int("Enter Duration in seconds: ", 1, 3600);
                safe_read_string("Enter Simulated File Path: ", s.file_path, MAX_PATH_LEN);
                s.is_favourite = safe_read_int("Mark as Favourite? (1 = Yes, 0 = No): ", 0, 1);
                s.priority = safe_read_int("Enter Playback Priority (1 to 10): ", 1, 10);

                if (add_song(&ctx->library, s)) {
                    sync_all_indices(ctx);
                    printf("[Success] Added song \"%s\" (ID: %d) to library.\n", s.title, s.id);
                }
                break;
            }
            case 3: {
                int id = safe_read_int("Enter Song ID to edit: ", 1, 99999);
                Song *s = find_song_by_id(&ctx->library, id);
                if (s == NULL) {
                    printf("[Error] Song ID %d not found!\n", id);
                    break;
                }
                printf("Editing Song: %s by %s\n", s->title, s->artist);
                safe_read_string("Enter New Title (or press enter to keep): ", s->title, MAX_TITLE_LEN);
                safe_read_string("Enter New Artist (or press enter to keep): ", s->artist, MAX_ARTIST_LEN);
                safe_read_string("Enter New Album: ", s->album, MAX_ALBUM_LEN);
                safe_read_string("Enter New Genre: ", s->genre, MAX_GENRE_LEN);
                s->duration = safe_read_int("Enter Duration in seconds: ", 1, 3600);
                s->is_favourite = safe_read_int("Mark as Favourite? (1 = Yes, 0 = No): ", 0, 1);
                s->priority = safe_read_int("Enter Priority (1-10): ", 1, 10);
                sync_all_indices(ctx);
                printf("[Success] Updated song details for ID %d.\n", id);
                break;
            }
            case 4: {
                int id = safe_read_int("Enter Song ID to delete: ", 1, 99999);
                Song *s = find_song_by_id(&ctx->library, id);
                if (s == NULL) {
                    printf("[Error] Song ID %d not found.\n", id);
                    break;
                }
                char title_copy[MAX_TITLE_LEN];
                strncpy(title_copy, s->title, MAX_TITLE_LEN - 1);
                cascade_delete_song(ctx, id, title_copy);
                break;
            }
            case 5: {
                int id = safe_read_int("Enter Song ID to lookup: ", 1, 99999);
                /* Demonstrate O(1) Hash Table lookup */
                int lib_idx = hash_search(&ctx->hash_table, id);
                if (lib_idx != -1 && lib_idx < ctx->library.count) {
                    printf("[Hash Table O(1) Match Found at Bucket Index]\n");
                    print_song_card(&ctx->library.songs[lib_idx]);
                } else {
                    printf("[Hash Table] Song ID %d not found.\n", id);
                }
                break;
            }
            case 6: {
                char title[MAX_TITLE_LEN];
                safe_read_string("Enter title keyword to search: ", title, MAX_TITLE_LEN);
                int idx = linear_search_by_title(&ctx->library, title);
                if (idx != -1) {
                    printf("[Linear Search Found Match at Index %d]:\n", idx);
                    print_song_card(&ctx->library.songs[idx]);
                } else {
                    printf("No song found with title containing '%s'.\n", title);
                }
                break;
            }
            case 7: {
                char artist[MAX_ARTIST_LEN];
                safe_read_string("Enter artist keyword to search: ", artist, MAX_ARTIST_LEN);
                int idx = linear_search_by_artist(&ctx->library, artist);
                if (idx != -1) {
                    printf("[Linear Search Found Match at Index %d]:\n", idx);
                    print_song_card(&ctx->library.songs[idx]);
                } else {
                    printf("No song found with artist containing '%s'.\n", artist);
                }
                break;
            }
            case 8: {
                char genre[MAX_GENRE_LEN];
                safe_read_string("Enter genre name (e.g. Rock, Pop, Jazz): ", genre, MAX_GENRE_LEN);
                filter_by_genre(&ctx->library, genre);
                break;
            }
            case 9:
                printf("\nTotal Songs in Catalog: %d\n", ctx->library.count);
                break;
        }
    }
}

/* 2. Playlist Management Submenu */
void handle_playlist_menu(AppContext *ctx) {
    while (1) {
        Playlist *active_pl = &ctx->playlists[ctx->active_playlist_idx];
        printf("\n--- Playlist Management (Active: \"%s\") ---\n", active_pl->name);
        printf(" 1. View Current Playlist (Doubly Linked List)\n");
        printf(" 2. Add Song to Playlist (End)\n");
        printf(" 3. Add Song to Playlist (Start)\n");
        printf(" 4. Insert Song at Specific Position\n");
        printf(" 5. Remove Song at Position (Records Undo)\n");
        printf(" 6. Change Song Order (Reorder Tracks)\n");
        printf(" 7. Switch Active Playlist\n");
        printf(" 8. Create New Playlist\n");
        printf(" 9. Rename Current Playlist\n");
        printf(" 10. Delete Current Playlist\n");
        printf(" 11. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-11]: ", 1, 11);
        if (choice == 11) break;

        switch (choice) {
            case 1:
                display_playlist(active_pl, &ctx->library);
                break;
            case 2: {
                int sid = safe_read_int("Enter Song ID to append to playlist: ", 1, 99999);
                if (find_song_by_id(&ctx->library, sid) == NULL) {
                    printf("[Error] Song ID %d does not exist in library!\n", sid);
                    break;
                }
                add_to_playlist_end(active_pl, sid);
                sync_all_indices(ctx);

                /* Record Undo action */
                UndoAction act = { ACTION_ADD_SONG, sid, active_pl->count, 0 };
                push_action(&ctx->undo_stack, act);

                printf("[Success] Added song to end of playlist. (Action recorded for Undo)\n");
                break;
            }
            case 3: {
                int sid = safe_read_int("Enter Song ID to prepend to playlist: ", 1, 99999);
                if (find_song_by_id(&ctx->library, sid) == NULL) {
                    printf("[Error] Song ID %d does not exist in library!\n", sid);
                    break;
                }
                add_to_playlist_start(active_pl, sid);
                sync_all_indices(ctx);

                UndoAction act = { ACTION_ADD_SONG, sid, 1, 0 };
                push_action(&ctx->undo_stack, act);

                printf("[Success] Added song to start of playlist. (Action recorded for Undo)\n");
                break;
            }
            case 4: {
                int sid = safe_read_int("Enter Song ID to insert: ", 1, 99999);
                if (find_song_by_id(&ctx->library, sid) == NULL) {
                    printf("[Error] Song ID %d does not exist in library!\n", sid);
                    break;
                }
                int pos = safe_read_int("Enter target position: ", 1, active_pl->count + 1);
                if (insert_into_playlist_at(active_pl, sid, pos)) {
                    sync_all_indices(ctx);
                    UndoAction act = { ACTION_ADD_SONG, sid, pos, 0 };
                    push_action(&ctx->undo_stack, act);
                    printf("[Success] Inserted song at position %d.\n", pos);
                }
                break;
            }
            case 5: {
                if (active_pl->count == 0) {
                    printf("[Notice] Playlist is already empty.\n");
                    break;
                }
                int pos = safe_read_int("Enter position of track to remove: ", 1, active_pl->count);
                int removed_id = 0;
                if (remove_from_playlist_at(active_pl, pos, &removed_id)) {
                    sync_all_indices(ctx);
                    UndoAction act = { ACTION_REMOVE_SONG, removed_id, pos, 0 };
                    push_action(&ctx->undo_stack, act);
                    printf("[Success] Removed track (ID: %d) from position %d. (Recorded for Undo)\n", removed_id, pos);
                }
                break;
            }
            case 6: {
                if (active_pl->count < 2) {
                    printf("[Notice] Need at least 2 tracks to reorder.\n");
                    break;
                }
                int from_pos = safe_read_int("Enter current track position to move: ", 1, active_pl->count);
                int to_pos = safe_read_int("Enter target destination position: ", 1, active_pl->count);
                if (move_song_in_playlist(active_pl, from_pos, to_pos)) {
                    sync_all_indices(ctx);
                    UndoAction act = { ACTION_MOVE_SONG, 0, to_pos, from_pos };
                    push_action(&ctx->undo_stack, act);
                    printf("[Success] Moved track from position %d to %d.\n", from_pos, to_pos);
                }
                break;
            }
            case 7: {
                printf("\n--- Available Playlists ---\n");
                for (int i = 0; i < ctx->playlist_count; i++) {
                    printf(" %d. %s (%d tracks)%s\n",
                           i + 1, ctx->playlists[i].name, ctx->playlists[i].count,
                           (i == ctx->active_playlist_idx) ? " [ACTIVE]" : "");
                }
                int sel = safe_read_int("Select playlist: ", 1, ctx->playlist_count);
                ctx->active_playlist_idx = sel - 1;
                sync_all_indices(ctx);
                printf("[Success] Switched active playlist to '%s'.\n", ctx->playlists[ctx->active_playlist_idx].name);
                break;
            }
            case 8: {
                if (ctx->playlist_count >= MAX_PLAYLISTS) {
                    printf("[Warning] Maximum playlist limit (%d) reached.\n", MAX_PLAYLISTS);
                    break;
                }
                char name[MAX_PLAYLIST_NAME];
                safe_read_string("Enter name for new playlist: ", name, MAX_PLAYLIST_NAME);
                init_playlist(&ctx->playlists[ctx->playlist_count], name);
                ctx->playlist_count++;
                printf("[Success] Created new playlist '%s'.\n", name);
                break;
            }
            case 9: {
                char name[MAX_PLAYLIST_NAME];
                safe_read_string("Enter new name for playlist: ", name, MAX_PLAYLIST_NAME);
                strncpy(active_pl->name, name, MAX_PLAYLIST_NAME - 1);
                active_pl->name[MAX_PLAYLIST_NAME - 1] = '\0';
                printf("[Success] Renamed playlist to '%s'.\n", active_pl->name);
                break;
            }
            case 10: {
                if (ctx->playlist_count <= 1) {
                    printf("[Warning] Cannot delete the only existing playlist.\n");
                    break;
                }
                clear_playlist(active_pl);
                for (int i = ctx->active_playlist_idx; i < ctx->playlist_count - 1; i++) {
                    ctx->playlists[i] = ctx->playlists[i + 1];
                }
                ctx->playlist_count--;
                ctx->active_playlist_idx = 0;
                sync_all_indices(ctx);
                printf("[Success] Playlist deleted.\n");
                break;
            }
        }
    }
}

/* 3. Music Player Simulator */
void handle_music_player_menu(AppContext *ctx) {
    Playlist *active_pl = &ctx->playlists[ctx->active_playlist_idx];

    /* If no song currently selected, point to head of active playlist */
    if (ctx->current_song_id == 0 && active_pl->head != NULL) {
        ctx->current_song_id = active_pl->head->song_id;
        active_pl->current = active_pl->head;
    }

    while (1) {
        Song *curr_song = find_song_by_id(&ctx->library, ctx->current_song_id);

        printf("\n============================================================\n");
        printf("                    NOW PLAYING SIMULATOR                   \n");
        printf("============================================================\n");
        if (curr_song != NULL) {
            char dur[16];
            format_duration(curr_song->duration, dur);
            printf(" Track:    %s (ID: %d)\n", curr_song->title, curr_song->id);
            printf(" Artist:   %s\n", curr_song->artist);
            printf(" Album:    %s\n", curr_song->album);
            printf(" Genre:    %s  |  Duration: %s\n", curr_song->genre, dur);
            printf(" Status:   %s  |  Priority: Level %d\n",
                   ctx->is_playing ? "[ PLAYING > ]" : "[ PAUSED || ]", curr_song->priority);
        } else {
            printf(" Track:    [ No Track Selected / Playlist Empty ]\n");
            printf(" Status:   [ STOPPED ]\n");
        }
        printf(" Playlist: \"%s\" (%d tracks)\n", active_pl->name, active_pl->count);
        printf(" Repeat:   [%s]  |  Upcoming Queue: [%d waiting]\n",
               ctx->repeat_playlist.is_repeat_enabled ? "ON (Circular)" : "OFF",
               ctx->upcoming_queue.count);
        printf("============================================================\n");
        printf(" [1] Play/Resume     [2] Pause          [3] Stop\n");
        printf(" [4] Next Track      [5] Previous Track [6] Toggle Repeat\n");
        printf(" [7] Play Priority   [8] Back to Main Menu\n");
        printf("============================================================\n");

        int choice = safe_read_int("Player Action [1-8]: ", 1, 8);
        if (choice == 8) break;

        switch (choice) {
            case 1:
                if (curr_song != NULL) {
                    ctx->is_playing = 1;
                    add_to_history(&ctx->history, curr_song->id);
                    printf("\n[Playing: \"%s\" by %s]\n", curr_song->title, curr_song->artist);
                } else {
                    printf("\n[Notice] No song available to play. Add songs to playlist first.\n");
                }
                break;
            case 2:
                ctx->is_playing = 0;
                printf("\n[Playback Paused]\n");
                break;
            case 3:
                ctx->is_playing = 0;
                printf("\n[Playback Stopped]\n");
                break;
            case 4: {
                /* Priority 1: Check Upcoming Queue (FIFO) */
                int queued_id = 0;
                if (dequeue(&ctx->upcoming_queue, &queued_id)) {
                    ctx->current_song_id = queued_id;
                    ctx->is_playing = 1;
                    add_to_history(&ctx->history, queued_id);
                    printf("\n[Playing Next Song from Upcoming Queue (FIFO)]\n");
                    break;
                }

                /* Priority 2: Navigate Playlist (Doubly or Circular Linked List) */
                if (ctx->repeat_playlist.is_repeat_enabled) {
                    Song *next_s = circular_next(&ctx->repeat_playlist, &ctx->library);
                    if (next_s != NULL) {
                        ctx->current_song_id = next_s->id;
                        ctx->is_playing = 1;
                        add_to_history(&ctx->history, next_s->id);
                    }
                } else {
                    Song *next_s = playlist_next(active_pl, &ctx->library);
                    if (next_s != NULL) {
                        ctx->current_song_id = next_s->id;
                        ctx->is_playing = 1;
                        add_to_history(&ctx->history, next_s->id);
                    }
                }
                break;
            }
            case 5: {
                if (ctx->repeat_playlist.is_repeat_enabled) {
                    Song *prev_s = circular_prev(&ctx->repeat_playlist, &ctx->library);
                    if (prev_s != NULL) {
                        ctx->current_song_id = prev_s->id;
                        ctx->is_playing = 1;
                        add_to_history(&ctx->history, prev_s->id);
                    }
                } else {
                    Song *prev_s = playlist_prev(active_pl, &ctx->library);
                    if (prev_s != NULL) {
                        ctx->current_song_id = prev_s->id;
                        ctx->is_playing = 1;
                        add_to_history(&ctx->history, prev_s->id);
                    }
                }
                break;
            }
            case 6:
                circular_toggle_repeat(&ctx->repeat_playlist);
                break;
            case 7: {
                int pq_id = 0, pq_pri = 0;
                if (pq_extract_max(&ctx->priority_queue, &pq_id, &pq_pri)) {
                    ctx->current_song_id = pq_id;
                    ctx->is_playing = 1;
                    add_to_history(&ctx->history, pq_id);
                    Song *s = find_song_by_id(&ctx->library, pq_id);
                    printf("\n[Priority Playback Activated! Playing Level %d Track: \"%s\"]\n",
                           pq_pri, s ? s->title : "Unknown");
                } else {
                    printf("\n[Notice] Priority Queue is empty. Assign priority to songs first.\n");
                }
                break;
            }
        }
    }
}

/* 4. Upcoming Queue Submenu */
void handle_upcoming_queue_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Upcoming Playback Queue (FIFO) ---\n");
        printf(" 1. View Upcoming Queue\n");
        printf(" 2. Add Song to Queue (Enqueue)\n");
        printf(" 3. Play Next Queued Song (Dequeue into Player)\n");
        printf(" 4. Remove Next Queued Song without Playing\n");
        printf(" 5. Clear Upcoming Queue\n");
        printf(" 6. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-6]: ", 1, 6);
        if (choice == 6) break;

        switch (choice) {
            case 1:
                display_queue(&ctx->upcoming_queue, &ctx->library);
                break;
            case 2: {
                int sid = safe_read_int("Enter Song ID to enqueue: ", 1, 99999);
                if (find_song_by_id(&ctx->library, sid) == NULL) {
                    printf("[Error] Song ID %d not in library.\n", sid);
                    break;
                }
                if (enqueue(&ctx->upcoming_queue, sid)) {
                    printf("[Success] Enqueued song ID %d to upcoming queue.\n", sid);
                }
                break;
            }
            case 3: {
                int sid = 0;
                if (dequeue(&ctx->upcoming_queue, &sid)) {
                    ctx->current_song_id = sid;
                    ctx->is_playing = 1;
                    add_to_history(&ctx->history, sid);
                    Song *s = find_song_by_id(&ctx->library, sid);
                    printf("[Playing Queued Track: \"%s\"]\n", s ? s->title : "Unknown");
                } else {
                    printf("[Notice] Upcoming queue is empty.\n");
                }
                break;
            }
            case 4: {
                int sid = 0;
                if (dequeue(&ctx->upcoming_queue, &sid)) {
                    printf("[Success] Removed song ID %d from queue.\n", sid);
                } else {
                    printf("[Notice] Upcoming queue is empty.\n");
                }
                break;
            }
            case 5:
                clear_queue(&ctx->upcoming_queue);
                printf("[Success] Upcoming queue cleared.\n");
                break;
        }
    }
}

/* 5. Recently Played Submenu */
void handle_recently_played_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Recently Played History (Singly Linked List) ---\n");
        printf(" 1. View Listening History (Newest First)\n");
        printf(" 2. Show Latest Played Song\n");
        printf(" 3. Search History for Song\n");
        printf(" 4. Clear History\n");
        printf(" 5. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-5]: ", 1, 5);
        if (choice == 5) break;

        switch (choice) {
            case 1:
                display_history(&ctx->history, &ctx->library);
                break;
            case 2: {
                int latest_id = get_latest_played_song_id(&ctx->history);
                if (latest_id != -1) {
                    Song *s = find_song_by_id(&ctx->library, latest_id);
                    printf("\nLatest Played Song: \"%s\" by %s (ID: %d)\n",
                           s ? s->title : "Unknown", s ? s->artist : "-", latest_id);
                } else {
                    printf("\n[Notice] No listening history recorded yet.\n");
                }
                break;
            }
            case 3: {
                int sid = safe_read_int("Enter Song ID to search in history: ", 1, 99999);
                int pos = search_history(&ctx->history, sid);
                if (pos > 0) {
                    printf("[Found] Song ID %d is at rank %d in recently played history.\n", sid, pos);
                } else {
                    printf("[Not Found] Song ID %d has not been played recently.\n", sid);
                }
                break;
            }
            case 4:
                clear_history(&ctx->history);
                printf("[Success] Listening history cleared.\n");
                break;
        }
    }
}

/* 6. Favourites and Priority Submenu */
void handle_favourites_priority_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Favourites and Priority Playback ---\n");
        printf(" 1. View All Favourite Songs (*)\n");
        printf(" 2. Toggle Favourite Status of a Song\n");
        printf(" 3. View Priority Playback Queue (Max-Heap Display)\n");
        printf(" 4. Add / Update Song Priority (1 to 10)\n");
        printf(" 5. Play Highest-Priority Song (Extract Max)\n");
        printf(" 6. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-6]: ", 1, 6);
        if (choice == 6) break;

        switch (choice) {
            case 1: {
                printf("\n--- Favourite Songs Catalog ---\n");
                printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
                printf("| ID   | Title                     | Artist               | Genre      | Dur.  | Fav | Pri |\n");
                printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
                int fav_count = 0;
                for (int i = 0; i < ctx->library.count; i++) {
                    if (ctx->library.songs[i].is_favourite) {
                        print_song_row(&ctx->library.songs[i]);
                        fav_count++;
                    }
                }
                printf("+------+---------------------------+----------------------+------------+-------+-----+-----+\n");
                printf("Total Favourites: %d\n\n", fav_count);
                break;
            }
            case 2: {
                int sid = safe_read_int("Enter Song ID to toggle favourite status: ", 1, 99999);
                Song *s = find_song_by_id(&ctx->library, sid);
                if (s != NULL) {
                    s->is_favourite = !s->is_favourite;
                    printf("[Success] Song \"%s\" is now %s.\n",
                           s->title, s->is_favourite ? "marked as Favourite (*)" : "removed from Favourites");
                } else {
                    printf("[Error] Song ID %d not found.\n", sid);
                }
                break;
            }
            case 3:
                pq_display(&ctx->priority_queue, &ctx->library);
                break;
            case 4: {
                int sid = safe_read_int("Enter Song ID: ", 1, 99999);
                Song *s = find_song_by_id(&ctx->library, sid);
                if (s == NULL) {
                    printf("[Error] Song ID %d not in library.\n", sid);
                    break;
                }
                int pri = safe_read_int("Enter Priority Score (1 = Lowest, 10 = VIP Highest): ", 1, 10);
                s->priority = pri;
                pq_remove_song(&ctx->priority_queue, sid);
                pq_insert(&ctx->priority_queue, sid, pri);
                printf("[Success] Priority for \"%s\" set to Level %d and added to Max-Heap.\n", s->title, pri);
                break;
            }
            case 5: {
                int sid = 0, pri = 0;
                if (pq_extract_max(&ctx->priority_queue, &sid, &pri)) {
                    ctx->current_song_id = sid;
                    ctx->is_playing = 1;
                    add_to_history(&ctx->history, sid);
                    Song *s = find_song_by_id(&ctx->library, sid);
                    printf("[VIP Playback] Playing Highest-Priority Song (Level %d): \"%s\" by %s\n",
                           pri, s ? s->title : "Unknown", s ? s->artist : "-");
                } else {
                    printf("[Notice] Priority queue is empty.\n");
                }
                break;
            }
        }
    }
}

/* 7. Search and Sort Submenu */
void handle_search_sort_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Search and Sort Engine ---\n");
        printf(" 1. Binary Search by ID (Requires Sorted ID array: O(log N))\n");
        printf(" 2. Linear Search by Title\n");
        printf(" 3. Linear Search by Artist\n");
        printf(" 4. Sort Catalog by Title (Selection Sort: A-Z)\n");
        printf(" 5. Sort Catalog by Artist (Insertion Sort: A-Z)\n");
        printf(" 6. Sort Catalog by Duration (Bubble Sort: Shortest First)\n");
        printf(" 7. Sort Catalog by ID (Bubble Sort: Ascending)\n");
        printf(" 8. Alphabetical In-order Traversal (Binary Search Tree)\n");
        printf(" 9. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-9]: ", 1, 9);
        if (choice == 9) break;

        switch (choice) {
            case 1: {
                sort_songs_by_id(&ctx->library);
                int sid = safe_read_int("Enter Song ID to search via Binary Search: ", 1, 99999);
                int idx = binary_search_by_id(&ctx->library, sid);
                if (idx != -1) {
                    printf("[Binary Search Match Found in O(log N) at Index %d]:\n", idx);
                    print_song_card(&ctx->library.songs[idx]);
                } else {
                    printf("[Binary Search] Song ID %d not found in catalog.\n", sid);
                }
                break;
            }
            case 2: {
                char title[MAX_TITLE_LEN];
                safe_read_string("Enter title keyword: ", title, MAX_TITLE_LEN);
                int idx = linear_search_by_title(&ctx->library, title);
                if (idx != -1) {
                    printf("[Linear Search Match Found at Index %d]:\n", idx);
                    print_song_card(&ctx->library.songs[idx]);
                } else {
                    printf("No song matched title '%s'.\n", title);
                }
                break;
            }
            case 3: {
                char artist[MAX_ARTIST_LEN];
                safe_read_string("Enter artist keyword: ", artist, MAX_ARTIST_LEN);
                int idx = linear_search_by_artist(&ctx->library, artist);
                if (idx != -1) {
                    printf("[Linear Search Match Found at Index %d]:\n", idx);
                    print_song_card(&ctx->library.songs[idx]);
                } else {
                    printf("No song matched artist '%s'.\n", artist);
                }
                break;
            }
            case 4:
                sort_songs_by_title(&ctx->library);
                sync_all_indices(ctx);
                printf("[Success] Catalog sorted by Title using Selection Sort.\n");
                display_all_songs(&ctx->library);
                break;
            case 5:
                sort_songs_by_artist(&ctx->library);
                sync_all_indices(ctx);
                printf("[Success] Catalog sorted by Artist using Insertion Sort.\n");
                display_all_songs(&ctx->library);
                break;
            case 6:
                sort_songs_by_duration(&ctx->library);
                sync_all_indices(ctx);
                printf("[Success] Catalog sorted by Duration using Bubble Sort.\n");
                display_all_songs(&ctx->library);
                break;
            case 7:
                sort_songs_by_id(&ctx->library);
                sync_all_indices(ctx);
                printf("[Success] Catalog sorted by Song ID using Bubble Sort.\n");
                display_all_songs(&ctx->library);
                break;
            case 8:
                bst_inorder(&ctx->bst_tree, &ctx->library);
                break;
        }
    }
}

/* 8. Recommendations Submenu */
void handle_recommendations_menu(AppContext *ctx) {
    while (1) {
        printf("\n--- Song Recommendations (Graph Adjacency List) ---\n");
        printf(" 1. View Direct Recommendations for a Song (1-Hop Neighbors)\n");
        printf(" 2. Breadth-First Search (BFS) Discovery\n");
        printf(" 3. Depth-First Search (DFS) Traversal\n");
        printf(" 4. Rebuild Recommendation Network from Catalog\n");
        printf(" 5. Back to Main Menu\n");

        int choice = safe_read_int("Enter option [1-5]: ", 1, 5);
        if (choice == 5) break;

        switch (choice) {
            case 1: {
                int sid = safe_read_int("Enter base Song ID for recommendations: ", 1, 99999);
                graph_display_connections(&ctx->recommendation_graph, sid, &ctx->library);
                break;
            }
            case 2: {
                int sid = safe_read_int("Enter starting Song ID for BFS: ", 1, 99999);
                graph_bfs(&ctx->recommendation_graph, sid, &ctx->library);
                break;
            }
            case 3: {
                int sid = safe_read_int("Enter starting Song ID for DFS: ", 1, 99999);
                graph_dfs(&ctx->recommendation_graph, sid, &ctx->library);
                break;
            }
            case 4:
                build_recommendation_graph(&ctx->recommendation_graph, &ctx->library);
                printf("[Success] Recommendation graph rebuilt with %d vertices.\n",
                       ctx->recommendation_graph.num_vertices);
                break;
        }
    }
}

/* 9. Undo Last Action (Stack LIFO) */
void handle_undo_operation(AppContext *ctx) {
    UndoAction act;
    if (!pop_action(&ctx->undo_stack, &act)) {
        printf("\n[Notice] No undo action available. Undo stack is empty.\n");
        return;
    }

    Playlist *pl = &ctx->playlists[ctx->active_playlist_idx];

    printf("\n============================================================\n");
    printf("                  UNDO PLAYLIST MODIFICATION                \n");
    printf("============================================================\n");

    switch (act.type) {
        case ACTION_ADD_SONG: {
            /* Inverse of adding a song at position is removing it */
            int removed_id = 0;
            if (remove_from_playlist_at(pl, act.position, &removed_id)) {
                sync_all_indices(ctx);
                Song *s = find_song_by_id(&ctx->library, act.song_id);
                printf("[Undo Executed] Reversed song addition: Removed \"%s\" from position %d.\n",
                       s ? s->title : "Unknown", act.position);
            }
            break;
        }
        case ACTION_REMOVE_SONG: {
            /* Inverse of removing a song is re-inserting it at its original position */
            if (insert_into_playlist_at(pl, act.song_id, act.position)) {
                sync_all_indices(ctx);
                Song *s = find_song_by_id(&ctx->library, act.song_id);
                printf("[Undo Executed] Restored removed song \"%s\" back to position %d.\n",
                       s ? s->title : "Unknown", act.position);
            }
            break;
        }
        case ACTION_MOVE_SONG: {
            /* Inverse of moving from prev->pos is moving from pos->prev */
            if (move_song_in_playlist(pl, act.position, act.previous_pos)) {
                sync_all_indices(ctx);
                printf("[Undo Executed] Reversed track move: Returned track to position %d.\n", act.previous_pos);
            }
            break;
        }
    }
    printf("============================================================\n");
    printf("Remaining actions on Undo stack: %d\n\n", get_stack_size(&ctx->undo_stack));
}

/* 10. DSA Demonstration Visualizer Submenu */
void handle_dsa_demonstration_menu(AppContext *ctx) {
    while (1) {
        printf("\n============================================================\n");
        printf("           DSA ARCHITECTURE DEMONSTRATION & VIVA            \n");
        printf("============================================================\n");
        printf("  [1]  Dynamic Array (Main Library, Resizing, O(1) Indexing)\n");
        printf("  [2]  Singly Linked List (History: Head Insertion & Tail Pruning)\n");
        printf("  [3]  Doubly Linked List (Playlist Navigation: Bidirectional)\n");
        printf("  [4]  Circular Linked List (Repeat Mode: Looping Cycle)\n");
        printf("  [5]  LIFO Stack (Undo System: Push/Pop/Underflow)\n");
        printf("  [6]  FIFO Queue (Upcoming Songs: Circular Wrap-Around)\n");
        printf("  [7]  Max-Heap Priority Queue (Priority Playback: O(log N))\n");
        printf("  [8]  Hash Table (Separate Chaining: O(1) Search & Collisions)\n");
        printf("  [9]  Binary Search Tree (Titles: Inorder, Preorder, Postorder)\n");
        printf("  [10] Graph Adjacency List (BFS & DFS Recommendations)\n");
        printf("  [11] Back to Main Menu\n");
        printf("============================================================\n");

        int choice = safe_read_int("Select Data Structure to inspect [1-11]: ", 1, 11);
        if (choice == 11) break;

        switch (choice) {
            case 1:
                printf("\n--- [1] DYNAMIC ARRAY DEMONSTRATION ---\n");
                printf("Count: %d | Allocated Capacity: %d | Element Size: %zu bytes\n",
                       ctx->library.count, ctx->library.capacity, sizeof(Song));
                display_all_songs(&ctx->library);
                break;
            case 2:
                printf("\n--- [2] SINGLY LINKED LIST DEMONSTRATION ---\n");
                display_history(&ctx->history, &ctx->library);
                break;
            case 3:
                printf("\n--- [3] DOUBLY LINKED LIST DEMONSTRATION ---\n");
                display_playlist(&ctx->playlists[ctx->active_playlist_idx], &ctx->library);
                break;
            case 4:
                printf("\n--- [4] CIRCULAR LINKED LIST DEMONSTRATION ---\n");
                display_circular_playlist(&ctx->repeat_playlist, &ctx->library);
                break;
            case 5:
                printf("\n--- [5] STACK DEMONSTRATION ---\n");
                printf("Stack Top Index: %d | Stack Capacity: %d | Elements: %d\n",
                       ctx->undo_stack.top, ctx->undo_stack.capacity, get_stack_size(&ctx->undo_stack));
                if (is_stack_empty(&ctx->undo_stack)) {
                    printf("Status: Stack is currently EMPTY (Underflow state if pop is attempted).\n");
                } else {
                    UndoAction top_act;
                    peek_action(&ctx->undo_stack, &top_act);
                    printf("Top Action: Type=%d, SongID=%d, Position=%d\n",
                           top_act.type, top_act.song_id, top_act.position);
                }
                break;
            case 6:
                printf("\n--- [6] FIFO QUEUE DEMONSTRATION ---\n");
                printf("Queue Front: %d | Queue Rear: %d | Count: %d / %d\n",
                       ctx->upcoming_queue.front, ctx->upcoming_queue.rear,
                       ctx->upcoming_queue.count, ctx->upcoming_queue.capacity);
                display_queue(&ctx->upcoming_queue, &ctx->library);
                break;
            case 7:
                printf("\n--- [7] MAX-HEAP PRIORITY QUEUE DEMONSTRATION ---\n");
                printf("Heap Size: %d | Heap Capacity: %d\n",
                       ctx->priority_queue.size, ctx->priority_queue.capacity);
                pq_display(&ctx->priority_queue, &ctx->library);
                break;
            case 8:
                printf("\n--- [8] HASH TABLE DEMONSTRATION ---\n");
                display_hash_table(&ctx->hash_table, &ctx->library);
                break;
            case 9:
                printf("\n--- [9] BINARY SEARCH TREE DEMONSTRATION ---\n");
                bst_inorder(&ctx->bst_tree, &ctx->library);
                bst_preorder(&ctx->bst_tree, &ctx->library);
                bst_postorder(&ctx->bst_tree, &ctx->library);
                break;
            case 10:
                printf("\n--- [10] GRAPH ADJACENCY LIST DEMONSTRATION ---\n");
                printf("Graph Vertices: %d | Capacity: %d\n",
                       ctx->recommendation_graph.num_vertices, ctx->recommendation_graph.capacity);
                if (ctx->library.count > 0) {
                    int start_id = ctx->library.songs[0].id;
                    graph_bfs(&ctx->recommendation_graph, start_id, &ctx->library);
                    graph_dfs(&ctx->recommendation_graph, start_id, &ctx->library);
                }
                break;
        }
    }
}
