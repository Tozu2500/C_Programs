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
    
}