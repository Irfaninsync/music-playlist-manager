#include <stdio.h>
#include <stdlib.h>
#include "../include/interface.h"

int main(void) {
    printf("============================================================\n");
    printf("     WELCOME TO THE B.TECH MUSIC PLAYLIST MANAGER (DSA)     \n");
    printf("============================================================\n");
    printf("Initializing 10 Data Structures...\n");

    AppContext ctx;
    init_app_context(&ctx);
    load_all_data(&ctx);

    printf("Application loaded successfully! Catalog has %d songs.\n", ctx.library.count);

    int running = 1;
    while (running) {
        print_main_menu();
        int choice = safe_read_int("Enter your choice [1-11]: ", 1, 11);

        switch (choice) {
            case 1:
                handle_music_library_menu(&ctx);
                break;
            case 2:
                handle_playlist_menu(&ctx);
                break;
            case 3:
                handle_music_player_menu(&ctx);
                break;
            case 4:
                handle_upcoming_queue_menu(&ctx);
                break;
            case 5:
                handle_recently_played_menu(&ctx);
                break;
            case 6:
                handle_favourites_priority_menu(&ctx);
                break;
            case 7:
                handle_search_sort_menu(&ctx);
                break;
            case 8:
                handle_recommendations_menu(&ctx);
                break;
            case 9:
                handle_undo_operation(&ctx);
                break;
            case 10:
                handle_dsa_demonstration_menu(&ctx);
                break;
            case 11:
                printf("\nSaving catalog data and cleaning up memory...\n");
                save_all_data(&ctx);
                free_app_context(&ctx);
                printf("[Success] Memory safely freed. Thank you for using Music Playlist Manager!\n\n");
                running = 0;
                break;
            default:
                printf("[!] Invalid choice. Please try again.\n");
                break;
        }
    }

    return 0;
}
