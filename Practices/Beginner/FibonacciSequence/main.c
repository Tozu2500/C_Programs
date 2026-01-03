#include <stdio.h>

int main() {

    int n;
    char choice;

    // Input validation
    do {
        unsigned long long first = 0, second = 1, next;

        printf("Fibonacci sequence generator\n\n");

        do {
            printf("Enter the number of terms: ");
            scanf("%d", &n);

            // Clear input buffer
            while (getchar() != '\n');

            if (n <= 0) {
                printf("Invalid input, please enter a positive number.\n\n");
            }
        } while (n <= 0);  // Keep asking until a valid input

        printf("\nFibonacci sequence (first %d terms):\n", n);

        // Generate and print fibonacci nums
        for (int i = 0; i < n; i++) {
            if (i <= 1) {
                next = i;
            } else {
                next = first + second;
                first = second;
                second = next;
            }

            printf("%llu ", next);
        }

        printf("\n\n");

        // Ask for another sequence
        printf("Generate again? (y/n): ");
        scanf(" %c", &choice);

        while (getchar() != '\n');

        printf("\n");

    } while (choice == 'y' || choice == 'Y');

    printf("There we go!\n");

    return 0;
}