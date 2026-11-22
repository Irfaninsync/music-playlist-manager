#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/graph.h"

/* Case-insensitive string equality helper */
static int str_equal_nocase(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0;
        }
        s1++;
        s2++;
    }
    return (*s1 == '\0' && *s2 == '\0');
}

/* Helper to find array index of a vertex by song_id */
static int find_vertex_index(const Graph *g, int song_id) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (g->vertices[i].song_id == song_id) {
            return i;
        }
    }
    return -1;
}

/* Initialize graph */
void init_graph(Graph *g, int capacity) {
    if (capacity <= 0) {
        capacity = DEFAULT_GRAPH_CAPACITY;
    }
    g->vertices = (GraphVertex *)malloc(sizeof(GraphVertex) * capacity);
    if (g->vertices == NULL) {
        printf("[Error] Memory allocation failed for Graph vertices!\n");
        g->num_vertices = 0;
        g->capacity = 0;
        return;
    }
    g->num_vertices = 0;
    g->capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        g->vertices[i].song_id = 0;
        g->vertices[i].head = NULL;
    }
}

/* Clear all vertices and adjacency lists */
void clear_graph(Graph *g) {
    if (g->vertices == NULL) return;

    for (int i = 0; i < g->num_vertices; i++) {
        AdjListNode *curr = g->vertices[i].head;
        while (curr != NULL) {
            AdjListNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        g->vertices[i].head = NULL;
        g->vertices[i].song_id = 0;
    }
    g->num_vertices = 0;
}

/* Free graph memory */
void free_graph(Graph *g) {
    if (g->vertices != NULL) {
        clear_graph(g);
        free(g->vertices);
        g->vertices = NULL;
    }
    g->num_vertices = 0;
    g->capacity = 0;
}

/* Add a vertex to the graph */
int graph_add_vertex(Graph *g, int song_id) {
    if (find_vertex_index(g, song_id) != -1) {
        return 0; /* Vertex already exists */
    }

    if (g->num_vertices >= g->capacity) {
        int new_capacity = g->capacity * 2;
        GraphVertex *temp = (GraphVertex *)realloc(g->vertices, sizeof(GraphVertex) * new_capacity);
        if (temp == NULL) {
            printf("[Error] Failed to expand graph vertices!\n");
            return 0;
        }
        g->vertices = temp;
        for (int i = g->capacity; i < new_capacity; i++) {
            g->vertices[i].song_id = 0;
            g->vertices[i].head = NULL;
        }
        g->capacity = new_capacity;
    }

    int idx = g->num_vertices;
    g->vertices[idx].song_id = song_id;
    g->vertices[idx].head = NULL;
    g->num_vertices++;
    return 1;
}

/* Add an undirected edge between src_id and dest_id */
void graph_add_edge(Graph *g, int src_id, int dest_id, const char *reason) {
    if (src_id == dest_id) return;

    int u = find_vertex_index(g, src_id);
    int v = find_vertex_index(g, dest_id);

    if (u == -1 || v == -1) return;

    /* Check if edge already exists to prevent duplicate edges */
    AdjListNode *check = g->vertices[u].head;
    while (check != NULL) {
        if (check->dest_song_id == dest_id) {
            return; /* Edge already exists */
        }
        check = check->next;
    }

    /* Add edge from u -> v */
    AdjListNode *node1 = (AdjListNode *)malloc(sizeof(AdjListNode));
    if (node1 != NULL) {
        node1->dest_song_id = dest_id;
        strncpy(node1->reason, reason, MAX_REASON_LEN - 1);
        node1->reason[MAX_REASON_LEN - 1] = '\0';
        node1->next = g->vertices[u].head;
        g->vertices[u].head = node1;
    }

    /* Add edge from v -> u (Undirected graph) */
    AdjListNode *node2 = (AdjListNode *)malloc(sizeof(AdjListNode));
    if (node2 != NULL) {
        node2->dest_song_id = src_id;
        strncpy(node2->reason, reason, MAX_REASON_LEN - 1);
        node2->reason[MAX_REASON_LEN - 1] = '\0';
        node2->next = g->vertices[v].head;
        g->vertices[v].head = node2;
    }
}

/* Build recommendation graph automatically based on shared Artist or Genre */
void build_recommendation_graph(Graph *g, const Library *lib) {
    clear_graph(g);

    /* 1. Add all songs as vertices */
    for (int i = 0; i < lib->count; i++) {
        graph_add_vertex(g, lib->songs[i].id);
    }

    /* 2. Connect songs sharing Artist or Genre */
    for (int i = 0; i < lib->count; i++) {
        for (int j = i + 1; j < lib->count; j++) {
            Song *s1 = &lib->songs[i];
            Song *s2 = &lib->songs[j];

            if (str_equal_nocase(s1->artist, s2->artist)) {
                graph_add_edge(g, s1->id, s2->id, "Same Artist");
            } else if (str_equal_nocase(s1->genre, s2->genre)) {
                char reason[MAX_REASON_LEN];
                snprintf(reason, sizeof(reason), "Genre: %s", s1->genre);
                graph_add_edge(g, s1->id, s2->id, reason);
            }
        }
    }
}

/* Remove a vertex from graph (e.g., when a song is deleted from library) */
void graph_remove_vertex(Graph *g, int song_id) {
    int target_idx = find_vertex_index(g, song_id);
    if (target_idx == -1) return;

    /* Free adjacency list of target vertex */
    AdjListNode *curr = g->vertices[target_idx].head;
    while (curr != NULL) {
        AdjListNode *temp = curr;
        curr = curr->next;
        free(temp);
    }

    /* Remove incoming edges pointing to song_id in other vertices */
    for (int i = 0; i < g->num_vertices; i++) {
        if (i == target_idx) continue;
        AdjListNode *node = g->vertices[i].head;
        AdjListNode *prev = NULL;

        while (node != NULL) {
            if (node->dest_song_id == song_id) {
                if (prev == NULL) {
                    g->vertices[i].head = node->next;
                } else {
                    prev->next = node->next;
                }
                AdjListNode *to_free = node;
                node = node->next;
                free(to_free);
            } else {
                prev = node;
                node = node->next;
            }
        }
    }

    /* Shift vertices array */
    for (int i = target_idx; i < g->num_vertices - 1; i++) {
        g->vertices[i] = g->vertices[i + 1];
    }
    g->num_vertices--;
}

/* Display direct 1-hop recommendations */
void graph_display_connections(const Graph *g, int song_id, const Library *lib) {
    int u = find_vertex_index(g, song_id);
    if (u == -1) {
        printf("[Notice] Song ID %d is not found in the recommendation graph.\n", song_id);
        return;
    }

    Song *base = find_song_by_id(lib, song_id);
    printf("\n============================================================\n");
    printf("   DIRECT RECOMMENDATIONS FOR: \"%s\" (ID: %d)\n",
           base ? base->title : "Unknown", song_id);
    printf("============================================================\n");

    AdjListNode *curr = g->vertices[u].head;
    if (curr == NULL) {
        printf(" No directly connected songs found in this catalog.\n");
        printf("============================================================\n\n");
        return;
    }

    int count = 1;
    while (curr != NULL) {
        Song *rec = find_song_by_id(lib, curr->dest_song_id);
        if (rec != NULL) {
            printf(" %d. [ID:%d] %-25s by %-18s [%s]\n",
                   count, rec->id, rec->title, rec->artist, curr->reason);
        }
        curr = curr->next;
        count++;
    }
    printf("============================================================\n\n");
}

/* Breadth-First Search (BFS) Traversal for Song Recommendations */
void graph_bfs(const Graph *g, int start_song_id, const Library *lib) {
    int start_idx = find_vertex_index(g, start_song_id);
    if (start_idx == -1) {
        printf("[Error] Start song ID %d not in graph.\n", start_song_id);
        return;
    }

    int *visited = (int *)calloc(g->num_vertices, sizeof(int));
    int *queue = (int *)malloc(sizeof(int) * g->num_vertices);
    if (!visited || !queue) {
        if (visited) free(visited);
        if (queue) free(queue);
        return;
    }

    int front = 0, rear = 0;

    /* Enqueue start vertex */
    visited[start_idx] = 1;
    queue[rear++] = start_idx;

    Song *start_song = find_song_by_id(lib, start_song_id);
    printf("\n============================================================\n");
    printf("   BREADTH-FIRST SEARCH (BFS) DISCOVERY (Start: \"%s\")\n",
           start_song ? start_song->title : "Unknown");
    printf("============================================================\n");

    int step = 1;
    while (front < rear) {
        int u = queue[front++];
        Song *s = find_song_by_id(lib, g->vertices[u].song_id);

        if (s != NULL) {
            printf(" Step %2d | [ID:%d] %-25s | Artist: %-18s | Genre: %s\n",
                   step++, s->id, s->title, s->artist, s->genre);
        }

        /* Explore adjacent neighbors */
        AdjListNode *curr = g->vertices[u].head;
        while (curr != NULL) {
            int v = find_vertex_index(g, curr->dest_song_id);
            if (v != -1 && !visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
            curr = curr->next;
        }
    }

    printf("============================================================\n");
    printf("BFS Traversal reached %d connected songs.\n\n", step - 1);

    free(visited);
    free(queue);
}

/* Recursive DFS helper */
static void dfs_recursive(const Graph *g, int u, int *visited, const Library *lib, int depth, int *step) {
    visited[u] = 1;
    Song *s = find_song_by_id(lib, g->vertices[u].song_id);

    if (s != NULL) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("-> Step %2d [ID:%d] %s (%s, %s)\n",
               (*step)++, s->id, s->title, s->artist, s->genre);
    }

    AdjListNode *curr = g->vertices[u].head;
    while (curr != NULL) {
        int v = find_vertex_index(g, curr->dest_song_id);
        if (v != -1 && !visited[v]) {
            dfs_recursive(g, v, visited, lib, depth + 1, step);
        }
        curr = curr->next;
    }
}

/* Depth-First Search (DFS) Traversal for Song Recommendations */
void graph_dfs(const Graph *g, int start_song_id, const Library *lib) {
    int start_idx = find_vertex_index(g, start_song_id);
    if (start_idx == -1) {
        printf("[Error] Start song ID %d not in graph.\n", start_song_id);
        return;
    }

    int *visited = (int *)calloc(g->num_vertices, sizeof(int));
    if (!visited) return;

    Song *start_song = find_song_by_id(lib, start_song_id);
    printf("\n============================================================\n");
    printf("   DEPTH-FIRST SEARCH (DFS) TRAVERSAL (Start: \"%s\")\n",
           start_song ? start_song->title : "Unknown");
    printf("============================================================\n");

    int step = 1;
    dfs_recursive(g, start_idx, visited, lib, 0, &step);

    printf("============================================================\n");
    printf("DFS Traversal completed.\n\n");

    free(visited);
}
