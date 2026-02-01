#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Clear all elements
void stack_clear(Stack *stack) {
    if (!stack) return;

    // Free any strings
    for (int i = 0; i <= stack->top; i++) {
        if (stack->elements[i].type == TYPE_STRING && stack->elements[i].data.str) {
            free(stack->elements[i].data.str);
        }
    }

    stack->top = -1;
}

// Creating a copy of the stack
Stack* stack_copy(const Stack *stack) {
    if (!stack) return NULL;

    Stack *new_stack = stack_create(stack->capacity, stack->max_capacity);
    if (!new_stack) return NULL;

    for (int i = 0; i <= stack->top; i++) {
        if (stack->elements[i].type == TYPE_STRING) {
            stack_push_string(new_stack, stack->elements[i].data.str);
        } else {
            // Copy element directly
            new_stack->top++;
            new_stack->elements[new_stack->top] = stack->elements[i];
            new_stack->total_pushes++;
        }
    }

    return new_stack;
}

// Print stack constants
void stack_print(const Stack *stack) {
    if (!stack) {
        printf("Stack is NULL\n");
        return;
    }

    if (stack_is_empty(stack)) {
        printf("Stack is empty\n");
        return;
    }

    printf("Stack (top -> bottom):\n");
    for (int i = stack->top; i >= 0; i--) {
        printf("  [%d]  ", i);

        switch (stack->elements[i].type) {
            case TYPE_INT:
                printf("int: %d\n", stack->elements[i].data.i);
                break;
            case TYPE_FLOAT:
                printf("float: %f\n", stack->elements[i].data.f);
                break;
            case TYPE_DOUBLE:
                printf("double: %d\n", stack->elements[i].data.d);
                break;
            case TYPE_CHAR:
                printf("char: '%c'\n", stack->elements[i].data.c);
                break;
            case TYPE_STRING:
                printf("string: \%s\"\n", stack->elements[i].data.str);
                break;
            case TYPE_POINTER:
                printf("pointer: %p\n", stack->elements[i].data.ptr);
                break;
        }
    }
}

// Print current stack stats
void stack_print_statistics(const Stack *stack) {
    if (!stack) {
        printf("Stack is NULL\n");
        return;
    }

    printf("\nStack Statistics\n");
    printf("Current stack size: %d\n", stack_size(stack));
    printf("Curren stack capacity: %d\n", stack->capacity);
    printf("Max stack capacity: %d %s \n", stack->max_capacity, stack->max_capacity == 0 ? "(unlimited)" : "");
    printf("Total stack pushes: %zu\n", stack->total_pushes);
    printf("Total stack pops: %zu\n", stack->total_pops);
    printf("Stack memory usage: %zu bytes\n", sizeof(Stack) + stack->capacity * sizeof(StackElement));
    printf("----------------------------------\n\n");
}

// Search for an element, this function returns position from the top, or -1 if the element was not found
bool stack_search(const Stack *stack, const StackElement *element, int *position) {
    if (!stack || !element || stack_is_empty(stack)) return false;

    for (int i = stack->top; i >= 0; i--) {
        if (stack->elements[i].type != element->type) continue;

        bool found = false;
        switch (element->type) {
            case TYPE_INT:
                found = stack->elements[i].data.i == element->data.i;
                break;
            case TYPE_FLOAT:
                found = stack->elements[i].data.f == element->data.f;
                break;
            case TYPE_DOUBLE:
                found = stack->elements[i].data.d == element->data.d;
                break;
            case TYPE_CHAR:
                found = stack->elements[i].data.c == element->data.c;
                break;
            case TYPE_STRING:
                found = strcmp(stack->elements[i].data.str, element->data.str) == 0;
                break;
            case TYPE_POINTER:
                found = stack->elements[i].data.ptr == element->data.ptr;
                break;
        }

        if (found) {
            if (position) *position = stack->top - i;
            return true;
        }
    }

    return false;
}

// Reverse the stack
void stack_reverse(Stack *stack) {
    if (!stack || stack_is_empty(stack)) return;

    int left = 0;
    int right = stack->top;

    while (left < right) {
        StackElement temp = stack->elements[left];
        stack->elements[left] = stack->elements[right];
        stack->elements[right] = temp;
        left++;
        right--;
    }
}

// Get an element at a specific index, 0 = bottom, top = stack->top
bool stack_get_at(const Stack *stack, int index, StackElement *element) {
    if (!stack || index < 0 || index > stack->top) return false;

    if (element) {
        *element = stack->elements[index];
    }

    return true;
}

// Count a specific type of elements
int stack_count_type(const Stack *stack, DataType type) {
    if (!stack || stack_is_empty(stack)) return 0;

    int count = 0;
    for (int i = 0; i <= stack->top; i++) {
        if (stack->elements[i].type == type) {
            count++;
        }
    }

    return count;
}