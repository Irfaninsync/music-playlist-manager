#ifndef BST_H
#define BST_H

#include "song.h"
#include "library.h"

/* Node of Binary Search Tree ordered by song title */
typedef struct BSTNode {
    int song_id;
    char title[MAX_TITLE_LEN];
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* Binary Search Tree structure */
typedef struct BST {
    BSTNode *root;
    int count;
} BST;

/* Lifecycle functions */
void init_bst(BST *tree);
void free_bst(BST *tree);
void clear_bst(BST *tree);

/* BST operations */
int bst_insert(BST *tree, int song_id, const char *title);
BSTNode* bst_search(const BST *tree, const char *title);
int bst_delete(BST *tree, const char *title);
void rebuild_bst(BST *tree, const Library *lib);

/* Traversals */
void bst_inorder(const BST *tree, const Library *lib);     /* Alphabetical Order (A-Z) */
void bst_preorder(const BST *tree, const Library *lib);    /* Root -> Left -> Right */
void bst_postorder(const BST *tree, const Library *lib);   /* Left -> Right -> Root */

#endif /* BST_H */
