#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Creating a new stack
Stack* stack_create(int initial_capacity, int max_capacity) {
    if (initial_capacity <= 0) {
        initial_capacity = INITIAL_CAPACITY;
    }

    Stack *stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        fprintf(stderr, "Error: Memory allocation for stack failed!\n");
        return NULL;
    }

    stack->elements = (StackElement*)malloc(initial_capacity * sizeof(StackElement));
    if (!stack->elements) {
        fprintf(stderr, "Error: Memory allocation for stack failed!\n");
        free(stack);
        return NULL;
    }

    stack->top = -1;
    stack->capacity = initial_capacity;
    stack->max_capacity = max_capacity;
    stack->total_pushes = 0;
    stack->total_pops = 0;

    return stack;
}

// Destroy the stack and free memory
void stack_destroy(Stack *stack) {
    if (!stack) return;

    // Free dynamic string looping
    for (int i = 0; i <= stack->top; i++) {
        if (stack->elements[i].type == TYPE_STRING && stack->elements[i].data.str) {
            free (stack->elements[i].data.str);
        }
    } 

    free(stack->elements);
    free(stack);
}

// Check for an empty stack
bool stack_is_empty(const Stack *stack) {
    return stack == NULL || stack->top == -1;
}

// Check if the stack is full
bool stack_is_full(const Stack *stack) {
    if (!stack) return true;
    if (stack->max_capacity == 0) return false;
    return stack->top >= stack->max_capacity - 1;
}

// Get current size
int stack_size(const Stack *stack) {
    return stack ? stack->top + 1 : 0;
}

// Get current capacity
int stack_capacity(const Stack *stack) {
    return stack ? stack->capacity : 0;
}

// Stack resizing
bool stack_resize(Stack *stack, int new_capacity) {
    if (!stack || new_capacity <= 0) return false;

    StackElement *new_elements = (StackElement*)realloc(stack->elements, new_capacity * sizeof(StackElement));

    if (!new_elements) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }

    stack->elements = new_elements;
    stack->capacity = new_capacity;

    // Adjusting the top if the new capacity if smaller
    if (stack->top >= new_capacity) {
        stack->top = new_capacity - 1;
    }

    return true;
}
