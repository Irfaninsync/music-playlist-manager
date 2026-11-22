#ifndef STACK_H
#define STACK_H

#define DEFAULT_STACK_CAPACITY 50

/* Types of reversible playlist actions */
typedef enum {
    ACTION_ADD_SONG,      /* Undone by removing the song */
    ACTION_REMOVE_SONG,   /* Undone by re-inserting the song */
    ACTION_MOVE_SONG      /* Undone by moving the song back */
} ActionType;

/* Structure to store details of an action */
typedef struct UndoAction {
    ActionType type;
    int song_id;
    int position;         /* 1-based position in playlist */
    int previous_pos;     /* Used for move actions */
} UndoAction;

/* LIFO Stack structure */
typedef struct Stack {
    UndoAction *items;
    int top;              /* Index of topmost element (-1 when empty) */
    int capacity;
} Stack;

/* Lifecycle functions */
void init_stack(Stack *s, int capacity);
void free_stack(Stack *s);
void clear_stack(Stack *s);

/* Stack operations */
int is_stack_empty(const Stack *s);
int is_stack_full(const Stack *s);
int push_action(Stack *s, UndoAction action);
int pop_action(Stack *s, UndoAction *out_action);
int peek_action(const Stack *s, UndoAction *out_action);
int get_stack_size(const Stack *s);

#endif /* STACK_H */
