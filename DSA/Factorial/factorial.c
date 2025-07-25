#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototype for factorial recursion
unsigned long long factorial(int n);

int main() {
    int n;
    unsigned long long result;

    // Seed the random number generator
    srand((unsigned long) time(NULL));

    // Generate a random number between 0 and 10000
    n = rand() % 10001; // Random number between 0 and 10000

    // Start the timer
    printf("Starting factorial calculation\n");
    
    printf("Random number generated: %d\n", n); // Output the random number
    // Start the clock
    clock_t start = clock();

    factorial(n); // Call the factorial function
    printf("Calculating factorial of %d...\n", n);

    // Stop the clock
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC; // Calculate elapsed time
    printf("Time taken to calculate factorial: %.6f seconds\n", time_spent); // Output the time taken

    printf("Factorial of %d is: %llu\n", n, result); // Result output

    return 0; // Return 0 to indicate successful execution
}

unsigned long long factorial(int n) {
    if (n == 0 || n == 1) {
        return 1; // Base case: factorial of 0 or 1 is 1
    } else {
        return n * factorial(n - 1); // Recursive case: n * factorial of (n - 1)
    }
}