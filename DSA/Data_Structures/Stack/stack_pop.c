#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

// Generic pop function
bool stack_pop(Stack *stack, StackElement *element) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Error, stack underflow (empty stack)\n");
        return false;
    }

    if (element) {
        *element = stack->elements[stack->top];
    } else {
        // If there is no element provided, free the string if needed
        if (stack->elements[stack->top].type == TYPE_STRING) {
            free(stack->elements[stack->top].data.str);
        }
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop integer
bool stack_pop_int(Stack *stack, int *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_INT) {
        fprintf(stderr, "Error, type mismatch (expected int)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.i;
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop float
bool stack_pop_float(Stack *stack, float *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_FLOAT) {
        fprintf(stderr, "Error, type mismatch (expected float)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.f;
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop a double
bool stack_pop_double(Stack *stack, double *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_DOUBLE) {
        fprintf(stderr, "Error, type mismatch (expected double)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.d;
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop character
bool stack_pop_char(Stack *stack, char *value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_CHAR) {
        fprintf(stderr, "Error, type mismatch (expected char)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.c;
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop a string (the caller is responsible for freeing)
bool stack_pop_string(Stack *stack, char **value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_STRING) {
        fprintf(stderr, "Error, type mismatch (expected string)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.str;
    } else {
        free(stack->elements[stack->top].data.str);
    }

    stack->top--;
    stack->total_pops++;

    return true;
}

// Pop a pointer
bool stack_pop_pointer(Stack *stack, void **value) {
    if (stack_is_empty(stack)) return false;

    if (stack->elements[stack->top].type != TYPE_POINTER) {
        fprintf(stderr, "Error, type mismatch (expected pointer)\n");
        return false;
    }

    if (value) {
        *value = stack->elements[stack->top].data.ptr;
    }

    stack->top--;
    stack->total_pops++;

    return true;
}