#include "stack.h"
#include <stdio.h>

// Peek at the top element without removing it
bool stack_peek(const Stack *stack, StackElement *element) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Error, cannot peek an empty stack\n");
        return false;
    }

    if (element) {
        *element = stack->elements[stack->top];
    }

    return true;
}

// Peek int
bool stack_peek_int(const Stack *stack, int *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_INT) {
        fprintf(stderr, "Error, type mismatch (expected int)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.i;
    }

    return true;
}

// Peek a character
bool stack_peek_char(const Stack *stack, char *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_CHAR) {
        fprintf(stderr, "Error, type mismatch (expected char)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.c;
    }

    return true;
}

// Peek float
bool stack_peek_float(const Stack *stack, float *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_FLOAT) {
        fprintf(stderr, "Error, type mismatch (expected float)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.f;
    }

    return true;
}

// Peek a double
bool stack_peek_double(const Stack *stack, double *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_DOUBLE) {
        fprintf(stderr, "Error, type mismatch (expected double)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.d;
    }

    return true;
}

// Get pointer to the top of the element
const StackElement* stack_peek_element(const Stack *stack) {
    if (stack_is_empty(stack)) return NULL;
    return &stack->elements[stack->top];
}