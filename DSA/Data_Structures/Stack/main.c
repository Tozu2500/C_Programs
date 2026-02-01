#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Stack Data Structure Demo\n\n\n");

    // Creating a stack, initial capacity = 5
    Stack *stack = stack_create(5, 0);

    printf("1. Test basic 'push' operation:\n");
    stack_push_int(stack, 10);
    stack_push_int(stack, 20);
    stack_push_int(stack, 30);
    stack_push_float(stack, 3.14f);
    stack_push_string(stack, "Testing stack!");
    stack_push_char(stack, 'A');

    stack_print(stack);
    stack_print_statistics(stack);

    // Test the peek function
    printf("2. Testing the peek function:\n");
    char c;
    if (stack_peek_char(stack, &c)) {
        printf("Top element (peek): '%c'\n\n", c);
    }

    // Testing the pop function
    printf("3. Testing pop operations:\n");
    stack_pop_char(stack, &c);
    printf("Popped char: '%c'\n", c);

    char *str;
    stack_pop_string(stack, &str);
    printf("Popped string: \"%s\"\n", str);
    free(str);  // Important!!

    float f;
    stack_pop_float(stack, &f);
    printf("Popped float: %f\n\n", f);

    stack_print(stack);

    // Test the search function
    printf("4. Testing search function:\n");
    StackElement search_elem;
    search_elem.type = TYPE_INT;
    search_elem.data.i = 20;
    int position;
    if (stack_search(stack, &search_elem, &position)) {
        printf("Found value 20 at position %d from top\n\n", position);
    }

    // Test the reverse stack function
    printf("5. Reversing the stack:\n");
    printf("Before: \n");
    stack_print(stack);

    stack_reverse(stack);
    printf("\nAfter reverse:\n");
    stack_print(stack);

    // Test stack copy
    printf("\n6. Test the stack copy:\n");
    Stack *stack_copy_obj = stack_copy(stack);
    printf("Original stack:\n");
    stack_print(stack);
    printf("\nCopied stack:\n");
    stack_print(stack_copy_obj);

    // Test type counting
    printf("\n7. Testing type counting:\n");
    printf("Number of integers in the stack: %d\n", stack_count_type(stack, TYPE_INT));

    // Test with max capacity
    printf("\n8. Testing stack with max capacity:\n");
    Stack *limited_stack = stack_create(2, 5);
    for (int i = 0; i <= 6; i++) {
        printf("Pushing %d: %s\n", i, stack_push_int(limited_stack, i) ? "Success" : "Failed (max capacity)");
    }

    stack_print(limited_stack);
    stack_print_statistics(limited_stack);

    // Cleanup
    stack_destroy(stack);
    stack_destroy(stack_copy_obj);
    stack_destroy(limited_stack);

    printf("\n Demo completed! \n");

    return 0;
}