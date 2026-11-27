#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SONGS 50
#define HASH_SIZE 11

/* --- 1. ARRAY: Main Music Library (Single Source of Truth) --- */
typedef struct Song {
    int id;
    char title[35], artist[25], genre[15];
    int duration, priority;
} Song;
Song library[MAX_SONGS];
int song_count = 0;

Song* get_song(int id) {
    for (int i = 0; i < song_count; i++)
        if (library[i].id == id) return &library[i];
    return NULL;
}

/* --- 2. SINGLY LINKED LIST: Listening History (O(1) Prepend) --- */
typedef struct SNode { int song_id; struct SNode *next; } SNode;
SNode *history_head = NULL;

void add_history(int id) {
    SNode *n = (SNode*)malloc(sizeof(SNode));
    n->song_id = id; n->next = history_head; history_head = n;
}

void show_history() {
    printf("\n--- Listening History (Newest First) ---\n");
    if (!history_head) { printf("[History Empty]\n"); return; }
    SNode *c = history_head; int r = 1;
    while (c && r <= 8) {
        Song *s = get_song(c->song_id);
        if (s) printf(" %d. [ID:%d] %s - %s\n", r++, s->id, s->title, s->artist);
        c = c->next;
    }
}

/* --- 3. DOUBLY & 4. CIRCULAR LINKED LIST: Playlist & Repeat Mode --- */
typedef struct DNode { int song_id; struct DNode *prev, *next; } DNode;
DNode *pl_head = NULL, *pl_tail = NULL, *pl_curr = NULL;
int pl_count = 0, repeat_mode = 0;

void playlist_add(int id) {
    DNode *n = (DNode*)malloc(sizeof(DNode));
    n->song_id = id; n->next = NULL; n->prev = pl_tail;
    if (!pl_head) pl_head = pl_tail = pl_curr = n;
    else { pl_tail->next = n; pl_tail = n; }
    pl_count++;
    if (repeat_mode) { pl_tail->next = pl_head; pl_head->prev = pl_tail; }
}

int playlist_remove_last() {
    if (!pl_tail) return 0;
    int rem_id = pl_tail->song_id;
    DNode *to_del = pl_tail;
    if (pl_head == pl_tail) pl_head = pl_tail = pl_curr = NULL;
    else {
        pl_tail = pl_tail->prev;
        if (repeat_mode) { pl_tail->next = pl_head; pl_head->prev = pl_tail; }
        else pl_tail->next = NULL;
    }
    free(to_del);
    pl_count--;
    return rem_id;
}

void toggle_repeat() {
    repeat_mode = !repeat_mode;
    if (pl_head && pl_tail) {
        if (repeat_mode) { pl_tail->next = pl_head; pl_head->prev = pl_tail; }
        else { pl_tail->next = NULL; pl_head->prev = NULL; }
    }
    printf("\n[Repeat Mode: %s]\n", repeat_mode ? "ON (Circular Loop)" : "OFF");
}

void show_playlist() {
    printf("\n--- Playlist (%d songs, Repeat: %s) ---\n", pl_count, repeat_mode ? "ON" : "OFF");
    if (!pl_head) { printf("[Playlist Empty]\n"); return; }
    DNode *c = pl_head; int idx = 1;
    do {
        Song *s = get_song(c->song_id);
        const char *cur = (c == pl_curr) ? " [PLAYING >]" : "";
        if (s) printf(" %d. [ID:%d] %-20s %-15s (%02d:%02d)%s\n",
                      idx++, s->id, s->title, s->artist, s->duration/60, s->duration%60, cur);
        c = c->next;
    } while (c && c != pl_head);
}

/* --- 5. STACK: Undo System (LIFO Action Reversal) --- */
int undo_stack[MAX_SONGS], undo_top = -1;
void push_undo(int id) { if (undo_top < MAX_SONGS - 1) undo_stack[++undo_top] = id; }
void pop_undo() {
    if (undo_top < 0) { printf("\n[Notice: Nothing to undo]\n"); return; }
    int last_id = undo_stack[undo_top--];
    playlist_remove_last();
    Song *s = get_song(last_id);
    printf("\n[Undo] Removed '%s' from playlist\n", s ? s->title : "Unknown");
}

/* --- 6. QUEUE: Upcoming Songs (FIFO Circular Array) --- */
int up_q[MAX_SONGS], q_front = 0, q_rear = -1, q_count = 0;
void q_enqueue(int id) {
    if (q_count >= MAX_SONGS) { printf("[Queue Full]\n"); return; }
    q_rear = (q_rear + 1) % MAX_SONGS;
    up_q[q_rear] = id; q_count++;
    printf("[Queued for upcoming playback]\n");
}
int q_dequeue() {
    if (q_count == 0) return 0;
    int id = up_q[q_front];
    q_front = (q_front + 1) % MAX_SONGS; q_count--;
    return id;
}
void show_queue() {
    printf("\n--- Upcoming Queue (FIFO) ---\n");
    if (q_count == 0) { printf("[Queue Empty]\n"); return; }
    int idx = q_front;
    for (int i = 0; i < q_count; i++) {
        Song *s = get_song(up_q[idx]);
        if (s) printf(" %d. [ID:%d] %s by %s\n", i + 1, s->id, s->title, s->artist);
        idx = (idx + 1) % MAX_SONGS;
    }
}

/* --- 7. PRIORITY QUEUE: VIP Playback (Binary Max-Heap) --- */
typedef struct { int song_id, priority; } HeapNode;
HeapNode heap[MAX_SONGS];
int heap_size = 0;

void heap_insert(int id, int pri) {
    int i = heap_size++;
    heap[i].song_id = id; heap[i].priority = pri;
    while (i > 0 && heap[i].priority > heap[(i - 1) / 2].priority) {
        HeapNode tmp = heap[i]; heap[i] = heap[(i - 1) / 2]; heap[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
}
int heap_extract_max(int *out_pri) {
    if (heap_size <= 0) return 0;
    int id = heap[0].song_id;
    if (out_pri) *out_pri = heap[0].priority;
    heap[0] = heap[--heap_size];
    int i = 0;
    while (2 * i + 1 < heap_size) {
        int left = 2 * i + 1, right = 2 * i + 2, best = left;
        if (right < heap_size && heap[right].priority > heap[left].priority) best = right;
        if (heap[best].priority <= heap[i].priority) break;
        HeapNode tmp = heap[i]; heap[i] = heap[best]; heap[best] = tmp;
        i = best;
    }
    return id;
}

/* --- 8. HASH TABLE: O(1) Song Search by ID (Separate Chaining) --- */
typedef struct HNode { int song_id, index; struct HNode *next; } HNode;
HNode *hash_table[HASH_SIZE];

void hash_insert(int id, int idx) {
    int b = id % HASH_SIZE;
    HNode *n = (HNode*)malloc(sizeof(HNode));
    n->song_id = id; n->index = idx; n->next = hash_table[b];
    hash_table[b] = n;
}
int hash_search(int id) {
    int b = id % HASH_SIZE;
    for (HNode *c = hash_table[b]; c; c = c->next)
        if (c->song_id == id) return c->index;
    return -1;
}

/* --- 9. BINARY SEARCH TREE: Alphabetical Titles (In-order A-Z) --- */
typedef struct BSTNode {
    int song_id;
    char title[35];
    struct BSTNode *left, *right;
} BSTNode;
BSTNode *bst_root = NULL;

BSTNode* bst_insert(BSTNode *root, int id, const char *title) {
    if (!root) {
        BSTNode *n = (BSTNode*)malloc(sizeof(BSTNode));
        n->song_id = id; strncpy(n->title, title, 34); n->title[34] = '\0';
        n->left = n->right = NULL;
        return n;
    }
    if (strcmp(title, root->title) < 0) root->left = bst_insert(root->left, id, title);
    else root->right = bst_insert(root->right, id, title);
    return root;
}
void bst_inorder(BSTNode *root) {
    if (!root) return;
    bst_inorder(root->left);
    Song *s = get_song(root->song_id);
    if (s) printf(" [ID:%-4d] %-22s %-16s (%s)\n", s->id, s->title, s->artist, s->genre);
    bst_inorder(root->right);
}

/* --- 10. GRAPH: Recommendations (Adjacency List - BFS) --- */
typedef struct GEdge { int dest_id; struct GEdge *next; } GEdge;
GEdge *adj[MAX_SONGS];

void graph_add_edge(int u_id, int v_id) {
    int u = -1, v = -1;
    for (int i = 0; i < song_count; i++) {
        if (library[i].id == u_id) u = i;
        if (library[i].id == v_id) v = i;
    }
    if (u == -1 || v == -1) return;
    GEdge *e1 = (GEdge*)malloc(sizeof(GEdge)); e1->dest_id = v_id; e1->next = adj[u]; adj[u] = e1;
    GEdge *e2 = (GEdge*)malloc(sizeof(GEdge)); e2->dest_id = u_id; e2->next = adj[v]; adj[v] = e2;
}
void build_graph() {
    for (int i = 0; i < song_count; i++) {
        for (int j = i + 1; j < song_count; j++) {
            if (strcmp(library[i].genre, library[j].genre) == 0 ||
                strcmp(library[i].artist, library[j].artist) == 0)
                graph_add_edge(library[i].id, library[j].id);
        }
    }
}
void graph_bfs(int start_id) {
    int visited[MAX_SONGS] = {0}, q[MAX_SONGS], f = 0, r = 0, s_idx = -1;
    for (int i = 0; i < song_count; i++) if (library[i].id == start_id) s_idx = i;
    if (s_idx == -1) return;
    visited[s_idx] = 1; q[r++] = s_idx;
    printf("\n--- BFS Recommendations from [ID:%d] ---\n", start_id);
    while (f < r) {
        int u = q[f++];
        Song *s = &library[u];
        printf(" -> [ID:%d] %s (%s, %s)\n", s->id, s->title, s->artist, s->genre);
        for (GEdge *e = adj[u]; e; e = e->next) {
            for (int k = 0; k < song_count; k++) {
                if (library[k].id == e->dest_id && !visited[k]) {
                    visited[k] = 1; q[r++] = k;
                }
            }
        }
    }
}

/* --- FILE PERSISTENCE & INITIALIZATION --- */
void load_songs(const char *fn) {
    FILE *fp = fopen(fn, "r");
    if (!fp) return;
    char line[256];
    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp) && song_count < MAX_SONGS) {
        Song s;
        if (sscanf(line, "%d,%34[^,],%24[^,],%*[^,],%14[^,],%d,%*[^,],%*d,%d",
                   &s.id, s.title, s.artist, s.genre, &s.duration, &s.priority) == 6) {
            library[song_count] = s;
            hash_insert(s.id, song_count);
            bst_root = bst_insert(bst_root, s.id, s.title);
            if (s.priority >= 8) heap_insert(s.id, s.priority);
            song_count++;
        }
    }
    fclose(fp);
    build_graph();
}

void display_library() {
    printf("\n=======================================================================\n");
    printf(" ID   | Title                     | Artist          | Genre     | Dur\n");
    printf("=======================================================================\n");
    for (int i = 0; i < song_count; i++) {
        Song *s = &library[i];
        printf(" %-4d | %-25s | %-15s | %-9s | %02d:%02d\n",
               s->id, s->title, s->artist, s->genre, s->duration/60, s->duration%60);
    }
    printf("=======================================================================\n");
}

/* --- MAIN PROGRAM & CONTROLLER --- */
int main() {
    load_songs("data/songs.csv");
    for (int i = 0; i < song_count && i < 3; i++) playlist_add(library[i].id);

    int choice = 0;
    while (choice != 11) {
        printf("\n============================================================\n");
        printf("      MUSIC PLAYLIST MANAGER (B.Tech DSA Team Project)      \n");
        printf("============================================================\n");
        printf(" 1. View Library (Array)       2. View Playlist (DLL)\n");
        printf(" 3. Add to Playlist            4. Play Next / Queue (FIFO)\n");
        printf(" 5. Play Previous Track        6. Toggle Repeat (CLL)\n");
        printf(" 7. Undo Last Add (Stack)      8. Upcoming Queue\n");
        printf(" 9. VIP Playback (Max-Heap)   10. Search & Recommend (BST/Graph)\n");
        printf(" 11. Exit\n");
        printf("============================================================\n");
        printf("Enter Choice (1-11): ");
        if (scanf("%d", &choice) != 1) { while(getchar() != '\n'); continue; }

        if (choice == 1) display_library();
        else if (choice == 2) show_playlist();
        else if (choice == 3) {
            int sid; printf("Song ID to add: "); scanf("%d", &sid);
            if (get_song(sid)) {
                playlist_add(sid); push_undo(sid);
                printf("[Added to Playlist & Recorded for Undo]\n");
            } else printf("[ID not found]\n");
        }
        else if (choice == 4) {
            int qid = q_dequeue();
            if (qid) {
                add_history(qid);
                Song *s = get_song(qid);
                printf("\n[Playing from Queue]: %s - %s\n", s->title, s->artist);
            } else if (pl_curr) {
                add_history(pl_curr->song_id);
                Song *s = get_song(pl_curr->song_id);
                printf("\n[Now Playing]: %s - %s\n", s->title, s->artist);
                if (pl_curr->next) pl_curr = pl_curr->next;
            }
        }
        else if (choice == 5) {
            if (pl_curr && pl_curr->prev) {
                pl_curr = pl_curr->prev;
                add_history(pl_curr->song_id);
                Song *s = get_song(pl_curr->song_id);
                printf("\n[Playing Previous]: %s - %s\n", s->title, s->artist);
            } else printf("[At beginning or playlist empty]\n");
        }
        else if (choice == 6) toggle_repeat();
        else if (choice == 7) pop_undo();
        else if (choice == 8) {
            int sub; printf("1. View Queue  2. Enqueue Song: "); scanf("%d", &sub);
            if (sub == 1) show_queue();
            else { int sid; printf("Song ID: "); scanf("%d", &sid); if (get_song(sid)) q_enqueue(sid); }
        }
        else if (choice == 9) {
            int pri = 0, sid = heap_extract_max(&pri);
            if (sid) {
                add_history(sid);
                Song *s = get_song(sid);
                printf("\n[VIP Priority %d Playback]: %s - %s\n", pri, s->title, s->artist);
            } else printf("[Priority Queue Empty]\n");
        }
        else if (choice == 10) {
            int sub; printf("1. Hash Table O(1)  2. BST Titles (A-Z)  3. Graph BFS: "); scanf("%d", &sub);
            if (sub == 1) {
                int sid; printf("Song ID: "); scanf("%d", &sid);
                int idx = hash_search(sid);
                if (idx != -1) printf("[Found]: %s by %s (%s)\n", library[idx].title, library[idx].artist, library[idx].genre);
                else printf("[Not found in Hash Table]\n");
            } else if (sub == 2) {
                printf("\n--- BST Titles (A-Z) ---\n");
                bst_inorder(bst_root);
            } else if (sub == 3) {
                int sid; printf("Starting Song ID: "); scanf("%d", &sid);
                graph_bfs(sid);
            }
        }
    }
    show_history();
    printf("\nExiting Music Playlist Manager. Thank you!\n");
    return 0;
}
