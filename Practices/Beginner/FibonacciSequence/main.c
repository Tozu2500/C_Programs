#include <stdio.h>

int main() {

    int n;
    char choice;

    do {
        unsigned long long first = 0, second = 1, next;

        printf("Fibonacci sequence generator\n\n");

        // User input with validation
        do {
            printf("Enter the number of terms: ");
            scanf("%d", &n);

            // Input buffer clear
            while (getchar() != '\n');

            // Check input
            if (n <= 0) {
                printf("Invalid input, please enter a positive number.\n\n");
            }

        } while (n <= 0);

        printf("\nFibonacci sequence (first %d terms): ", n);

        // Generate and print the sequence
        for (int i = 0; i < n; i++) {
            // First terms are 0 and 1
            if (i <= 1) {
                next = i;
            } else {
                // Each subsequent term is the sum of the previous two
                next = first + second;
                first = second;
                second = next;
            }

            printf("%llu ", next);
        }

        printf("\n\n");

        printf("Generate again? (y/n): ");
        scanf(" %c", &choice);

        while (getchar() != '\n');

        printf("\n");

    } while (choice == 'y' || choice == 'Y');

    printf("There we go! Sequence(s) generated\n");

    return 0;
}
