#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/bst.h"

/* Helper for case-insensitive title comparison */
static int compare_titles(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = (char)tolower((unsigned char)*s1);
        char c2 = (char)tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/* Recursive node creation */
static BSTNode* create_node(int song_id, const char *title) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        printf("[Error] Memory allocation failed for BST node!\n");
        return NULL;
    }
    node->song_id = song_id;
    strncpy(node->title, title, MAX_TITLE_LEN - 1);
    node->title[MAX_TITLE_LEN - 1] = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* Recursive insertion */
static BSTNode* insert_recursive(BSTNode *root, int song_id, const char *title, int *inserted) {
    if (root == NULL) {
        *inserted = 1;
        return create_node(song_id, title);
    }

    int cmp = compare_titles(title, root->title);
    if (cmp < 0) {
        root->left = insert_recursive(root->left, song_id, title, inserted);
    } else if (cmp > 0) {
        root->right = insert_recursive(root->right, song_id, title, inserted);
    } else {
        /* Duplicate title: use song_id as secondary key */
        if (song_id < root->song_id) {
            root->left = insert_recursive(root->left, song_id, title, inserted);
        } else if (song_id > root->song_id) {
            root->right = insert_recursive(root->right, song_id, title, inserted);
        } else {
            *inserted = 0; /* Identical song already exists */
        }
    }
    return root;
}

/* Recursive search */
static BSTNode* search_recursive(BSTNode *root, const char *title) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = compare_titles(title, root->title);
    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return search_recursive(root->left, title);
    } else {
        return search_recursive(root->right, title);
    }
}

/* Find minimum node in subtree (In-order successor) */
static BSTNode* find_min(BSTNode *node) {
    while (node && node->left != NULL) {
        node = node->left;
    }
    return node;
}

/* Recursive deletion */
static BSTNode* delete_recursive(BSTNode *root, const char *title, int *deleted) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = compare_titles(title, root->title);
    if (cmp < 0) {
        root->left = delete_recursive(root->left, title, deleted);
    } else if (cmp > 0) {
        root->right = delete_recursive(root->right, title, deleted);
    } else {
        /* Node found! Handle the 3 deletion cases */
        *deleted = 1;

        /* Case 1 & 2: 0 or 1 child */
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }

        /* Case 3: 2 children - get in-order successor (min in right subtree) */
        BSTNode *successor = find_min(root->right);
        root->song_id = successor->song_id;
        strncpy(root->title, successor->title, MAX_TITLE_LEN - 1);
        root->title[MAX_TITLE_LEN - 1] = '\0';

        /* Delete the successor from right subtree */
        root->right = delete_recursive(root->right, successor->title, deleted);
    }
    return root;
}

/* Recursive post-order deallocation */
static void free_recursive(BSTNode *root) {
    if (root != NULL) {
        free_recursive(root->left);
        free_recursive(root->right);
        free(root);
    }
}

/* Recursive In-order Traversal (LNR -> Sorted A-Z) */
static void inorder_recursive(BSTNode *root, const Library *lib, int *counter) {
    if (root != NULL) {
        inorder_recursive(root->left, lib, counter);

        Song *s = find_song_by_id(lib, root->song_id);
        if (s != NULL) {
            char dur[16];
            format_duration(s->duration, dur);
            char short_artist[19];
            strncpy(short_artist, s->artist, 18);
            short_artist[18] = '\0';

            printf(" %-4d | %-4d | %-28s | %-18s | %-6s\n",
                   *counter, s->id, s->title, short_artist, dur);
        } else {
            printf(" %-4d | %-4d | %-28s | -                  | -\n",
                   *counter, root->song_id, root->title);
        }
        (*counter)++;

        inorder_recursive(root->right, lib, counter);
    }
}

/* Recursive Pre-order Traversal (NLR -> Root, Left, Right) */
static void preorder_recursive(BSTNode *root, int depth) {
    if (root != NULL) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("-> [ID:%d] %s\n", root->song_id, root->title);
        preorder_recursive(root->left, depth + 1);
        preorder_recursive(root->right, depth + 1);
    }
}

/* Recursive Post-order Traversal (LRN -> Left, Right, Root) */
static void postorder_recursive(BSTNode *root, int depth) {
    if (root != NULL) {
        postorder_recursive(root->left, depth + 1);
        postorder_recursive(root->right, depth + 1);
        for (int i = 0; i < depth; i++) printf("  ");
        printf("-> [ID:%d] %s\n", root->song_id, root->title);
    }
}

/* Public Functions */
void init_bst(BST *tree) {
    tree->root = NULL;
    tree->count = 0;
}

void clear_bst(BST *tree) {
    free_recursive(tree->root);
    tree->root = NULL;
    tree->count = 0;
}

void free_bst(BST *tree) {
    clear_bst(tree);
}

int bst_insert(BST *tree, int song_id, const char *title) {
    int inserted = 0;
    tree->root = insert_recursive(tree->root, song_id, title, &inserted);
    if (inserted) {
        tree->count++;
    }
    return inserted;
}

BSTNode* bst_search(const BST *tree, const char *title) {
    return search_recursive(tree->root, title);
}

int bst_delete(BST *tree, const char *title) {
    int deleted = 0;
    tree->root = delete_recursive(tree->root, title, &deleted);
    if (deleted) {
        tree->count--;
    }
    return deleted;
}

void rebuild_bst(BST *tree, const Library *lib) {
    clear_bst(tree);
    for (int i = 0; i < lib->count; i++) {
        bst_insert(tree, lib->songs[i].id, lib->songs[i].title);
    }
}

void bst_inorder(const BST *tree, const Library *lib) {
    printf("\n============================================================\n");
    printf("     BST IN-ORDER TRAVERSAL: ALPHABETICAL ORDER (A - Z)     \n");
    printf("============================================================\n");
    printf(" %-4s | %-4s | %-28s | %-18s | %-6s\n", "No.", "ID", "Title", "Artist", "Dur.");
    printf("------+------+------------------------------+--------------------+--------\n");

    if (tree->root == NULL) {
        printf(" [BST is empty]\n");
        printf("============================================================\n\n");
        return;
    }

    int counter = 1;
    inorder_recursive(tree->root, lib, &counter);
    printf("============================================================\n");
    printf("Total Alphabetically Sorted Songs: %d\n\n", tree->count);
}

void bst_preorder(const BST *tree, const Library *lib) {
    (void)lib; /* Unused parameter */
    printf("\n--- BST Pre-order Traversal (Root -> Left -> Right) ---\n");
    if (tree->root == NULL) {
        printf("[BST is empty]\n");
        return;
    }
    preorder_recursive(tree->root, 0);
    printf("------------------------------------------------------\n\n");
}

void bst_postorder(const BST *tree, const Library *lib) {
    (void)lib; /* Unused parameter */
    printf("\n--- BST Post-order Traversal (Left -> Right -> Root) ---\n");
    if (tree->root == NULL) {
        printf("[BST is empty]\n");
        return;
    }
    postorder_recursive(tree->root, 0);
    printf("------------------------------------------------------\n\n");
}
