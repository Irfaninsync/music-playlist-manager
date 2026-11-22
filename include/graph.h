#ifndef GRAPH_H
#define GRAPH_H

#include "library.h"

#define DEFAULT_GRAPH_CAPACITY 50
#define MAX_REASON_LEN 40

/* Node in the adjacency list representing an edge */
typedef struct AdjListNode {
    int dest_song_id;               /* Target song ID */
    char reason[MAX_REASON_LEN];    /* "Same Artist" or "Same Genre: Rock" */
    struct AdjListNode *next;       /* Pointer to next neighbor */
} AdjListNode;

/* Vertex in the graph representing a song */
typedef struct GraphVertex {
    int song_id;                    /* Song ID of this vertex */
    AdjListNode *head;              /* Head of adjacency list */
} GraphVertex;

/* Graph represented via Adjacency List */
typedef struct Graph {
    GraphVertex *vertices;          /* Array of vertices */
    int num_vertices;               /* Number of vertices */
    int capacity;                   /* Allocated capacity */
} Graph;

/* Lifecycle functions */
void init_graph(Graph *g, int capacity);
void free_graph(Graph *g);
void clear_graph(Graph *g);

/* Graph construction */
int graph_add_vertex(Graph *g, int song_id);
void graph_add_edge(Graph *g, int src_id, int dest_id, const char *reason);
void build_recommendation_graph(Graph *g, const Library *lib);
void graph_remove_vertex(Graph *g, int song_id);

/* Graph exploration & recommendations */
void graph_display_connections(const Graph *g, int song_id, const Library *lib);
void graph_bfs(const Graph *g, int start_song_id, const Library *lib);
void graph_dfs(const Graph *g, int start_song_id, const Library *lib);

#endif /* GRAPH_H */
