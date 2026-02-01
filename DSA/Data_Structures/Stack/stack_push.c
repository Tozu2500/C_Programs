#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Internal helper function to push generic elements
static bool stack_push_element(Stack *stack, const StackElement *element) {
    if (!stack || !element) return false;

    // Check for a full stack
    if (stack->max_capacity > 0 && stack->top >= stack->max_capacity - 1) {
        fprintf(stderr, "Error, stack overflow (max capacity reached)\n");
        return false;
    }

    // Resize
    if (stack->top >= stack->capacity - 1) {
        int new_capacity = stack->capacity * GROWTH_FACTOR;

        // Check for max capacity
        if (stack->max_capacity > 0 && new_capacity > stack->max_capacity) {
            new_capacity = stack->max_capacity;
        }

        if (!stack_resize(stack, new_capacity)) {
            return false;
        }
    }

    stack->top++;
    stack->elements[stack->top] = *element;
    stack->total_pushes++;

    return true;
}

// Push int
bool stack_push_int(Stack *stack, int value) {
    StackElement element;
    element.data.i = value;
    element.type = TYPE_INT;
    return stack_push_element(stack, &element);
}

// Push float
bool stack_push_float(Stack *stack, float value) {
    StackElement element;
    element.data.f = value;
    element.type = TYPE_FLOAT;
    return stack_push_element(stack, &element);
}

// Push double
bool stack_push_double(Stack *stack, double value) {
    StackElement element;
    element.data.d = value;
    element.type = TYPE_DOUBLE;
    return stack_push_element(stack, &element);
}

// Push char
bool stack_push_char(Stack *stack, char value) {
    StackElement element;
    element.data.c = value;
    element.type = TYPE_CHAR;
    return stack_push_element(stack, &element);
}

// Pushing a string (copy)
bool stack_push_string(Stack *stack, const char *value) {
    if (!value) return false;

    StackElement element;
    element.data.str = (char*)malloc(strlen(value) + 1);
    if (!element.data.str) {
        fprintf(stderr, "Error, memory allocation failed!\n");
        return false;
    }
    strcpy(element.data.str, value);
    element.type = TYPE_STRING;

    if (!stack_push_element(stack, &element)) {
        free(element.data.str);
        return false;
    }

    return true;
}

// Pushing a pointer
bool stack_push_pointer(Stack *stack, void *value) {
    StackElement element;
    element.data.ptr = value;
    element.type = TYPE_POINTER;
    return stack_push_element(stack, &element);
}
