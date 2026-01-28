#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>

// Initial capacity and growth factor for dynamic stack
#define INITIAL_CAPACITY 10
#define GROWTH_FACTOR 2

// Stack element can hold different data
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_POINTER
} DataType;

// Union holding different data types
typedef union {
    int i;
    float f;
    double d;
    char c;
    char *str;
    void *ptr;
} StackData;

// Stack element structure
typedef struct {
    StackData data;
    DataType type;
} StackElement;

// Stack structure
typedef struct {
    StackElement *elements;  // Dynamic array of elements
    int top;  // Index of the top element, -1 if it is empty
    int capacity;  // Current capacity
    int max_capacity;  // Maximum capacity (0 for unlimited capacity)
    size_t total_pushes;  // Statistical
    size_t total_pops;  // Statistical
} Stack;

// Core operations
Stack* stack_create(int initial_capacity, int max_capacity);
void stack_destroy(Stack *stack);
bool stack_is_empty(const Stack *stack);
bool stack_is_full(const Stack *stack);
int stack_size(const Stack *stack);
int stack_capacity(const Stack *stack);
bool stack_resize(Stack *stack, int new_capacity);

#endif // STACK_H
