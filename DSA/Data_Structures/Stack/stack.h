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

// Push operations (stack_push.c)
bool stack_push_int(Stack *stack, int value);
bool stack_push_float(Stack *stack, float value);
bool stack_push_double(Stack *stack, double value);
bool stack_push_char(Stack *stack, char value);
bool stack_push_string(Stack *stack, const char *value);
bool stack_push_pointer(Stack *stack, void *value);

// Pop operations (stack_pop.c)
bool stack_pop(Stack *stack, StackElement *element);
bool stack_pop_int(Stack *stack, int *value);
bool stack_pop_float(Stack *stack, float *value);
bool stack_pop_double(Stack *stack, double *value);
bool stack_pop_char(Stack *stack, char *value);
bool stack_pop_string(Stack *stack, char **value);
bool stack_pop_pointer(Stack *stack, void **value);

// Peek operations (stack_peek.c)
bool stack_peek(const Stack *stack, StackElement *element);
bool stack_peek_int(const Stack *stack, int *value);
bool stack_peek_char(const Stack *stack, char *value);
bool stack_peek_float(const Stack *stack, float *value);
bool stack_peek_double(const Stack *stack, double *value);
const StackElement* stack_peek_element(const Stack *stack);

// Utility operations (stack_util.c)
void stack_clear(Stack *stack);
Stack* stack_copy(const Stack *stack);
void stack_print(const Stack *stack);
void stack_print_statistics(const Stack *stack);
bool stack_search(const Stack *stack, const StackElement *element, int *position);
void stack_reverse(Stack *stack);
bool stack_get_at(const Stack* stack, int index, StackElement *element);
int stack_count_type(const Stack *stack, DataType type);

#endif // STACK_H
