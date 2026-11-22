#ifndef INTERFACE_H
#define INTERFACE_H

#include "song.h"
#include "library.h"
#include "singly_linked_list.h"
#include "doubly_linked_list.h"
#include "circular_linked_list.h"
#include "stack.h"
#include "queue.h"
#include "priority_queue.h"
#include "hash_table.h"
#include "bst.h"
#include "graph.h"
#include "file_handler.h"

#define MAX_PLAYLISTS 10

/* Central application context holding all data structures */
typedef struct AppContext {
    Library library;
    HistoryList history;
    Playlist playlists[MAX_PLAYLISTS];
    int playlist_count;
    int active_playlist_idx;
    CircularPlaylist repeat_playlist;
    Stack undo_stack;
    Queue upcoming_queue;
    PriorityQueue priority_queue;
    HashTable hash_table;
    BST bst_tree;
    Graph recommendation_graph;
    
    /* Playback status */
    int is_playing;
    int current_song_id;
} AppContext;

/* Core Context Management */
void init_app_context(AppContext *ctx);
void free_app_context(AppContext *ctx);
void sync_all_indices(AppContext *ctx);

/* Safe Console Input Helpers */
int safe_read_int(const char *prompt, int min_val, int max_val);
void safe_read_string(const char *prompt, char *buffer, int max_len);

/* Menu Routines */
void print_main_menu(void);
void handle_music_library_menu(AppContext *ctx);
void handle_playlist_menu(AppContext *ctx);
void handle_music_player_menu(AppContext *ctx);
void handle_upcoming_queue_menu(AppContext *ctx);
void handle_recently_played_menu(AppContext *ctx);
void handle_favourites_priority_menu(AppContext *ctx);
void handle_search_sort_menu(AppContext *ctx);
void handle_recommendations_menu(AppContext *ctx);
void handle_undo_operation(AppContext *ctx);
void handle_dsa_demonstration_menu(AppContext *ctx);

/* Persistence helpers */
void save_all_data(AppContext *ctx);
void load_all_data(AppContext *ctx);

#endif /* INTERFACE_H */
