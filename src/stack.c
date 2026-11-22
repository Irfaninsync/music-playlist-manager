#include <stdio.h>
#include <stdlib.h>
#include "../include/stack.h"

/* Initialize the stack */
void init_stack(Stack *s, int capacity) {
    if (capacity <= 0) {
        capacity = DEFAULT_STACK_CAPACITY;
    }
    s->items = (UndoAction *)malloc(sizeof(UndoAction) * capacity);
    if (s->items == NULL) {
        printf("[Error] Memory allocation failed for Undo Stack!\n");
        s->top = -1;
        s->capacity = 0;
        return;
    }
    s->top = -1;
    s->capacity = capacity;
}

/* Free allocated stack memory */
void free_stack(Stack *s) {
    if (s->items != NULL) {
        free(s->items);
        s->items = NULL;
    }
    s->top = -1;
    s->capacity = 0;
}

/* Clear all actions from stack */
void clear_stack(Stack *s) {
    s->top = -1;
}

/* Check if stack is empty */
int is_stack_empty(const Stack *s) {
    return (s->top == -1);
}

/* Check if stack is full */
int is_stack_full(const Stack *s) {
    return (s->top >= s->capacity - 1);
}

/* Push an action onto the stack (LIFO) */
int push_action(Stack *s, UndoAction action) {
    if (is_stack_full(s)) {
        /* Dynamically resize stack if capacity is reached */
        int new_capacity = s->capacity * 2;
        UndoAction *temp = (UndoAction *)realloc(s->items, sizeof(UndoAction) * new_capacity);
        if (temp == NULL) {
            printf("[Warning] Stack full and unable to expand memory. Action not recorded.\n");
            return 0;
        }
        s->items = temp;
        s->capacity = new_capacity;
    }

    s->top++;
    s->items[s->top] = action;
    return 1;
}

/* Pop the most recent action from the stack */
int pop_action(Stack *s, UndoAction *out_action) {
    if (is_stack_empty(s)) {
        return 0; /* Stack Underflow: Nothing to pop */
    }

    if (out_action != NULL) {
        *out_action = s->items[s->top];
    }
    s->top--;
    return 1;
}

/* Peek at the topmost action without removing it */
int peek_action(const Stack *s, UndoAction *out_action) {
    if (is_stack_empty(s)) {
        return 0;
    }
    if (out_action != NULL) {
        *out_action = s->items[s->top];
    }
    return 1;
}

/* Return current count of actions on stack */
int get_stack_size(const Stack *s) {
    return s->top + 1;
}
