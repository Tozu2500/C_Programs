#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototype for fibonacci
void fibonacci(int n);

int main() {

    int n = 20; // Change this value to compute Fibonacci for different numbers
    printf("Computing Fibonacci for n = %d\n", n);

    // Save current time before computing Fibonacci
    time_t start, end;
    time(&start);

    printf("Start time: %s", ctime(&start));
    fibonacci(n); // Call the fibonacci function

    time(&end);

    printf("End time: %s", ctime(&end));
    double elapsedTime = difftime(end, start);
    printf("Elapsed time was: %.6f seconds\n", elapsedTime);

    printf("Fibonacci of %d is computed.\n", n);

    return 0;
}

void fibonacci(int n) {
    if (n < 0) {
        printf("Fibonacci of negative numbers is not defined.\n");
        return;
    } else if (n == 0) {
        printf("Fibonacci of 0 is 0\n");
        return;
    } else if (n == 1) {
        printf("Fibonacci of 1 is 1\n");
        return;
    } else if (n == 2) {
        printf("Fibonacci of 2 is 1\n");
        return;
    } else {
        int a = 0, b = 1, c;
        printf("Fibonacci sequence up to %d: ", n);
        printf("%lld %lld ", a, b);
        for (int i = 2; i < n; i++) {
            c = a + b; // Calculate the next Fibonacci number
            printf("%lld ", c);
            a = b; // Update a to to be the previous b
            b = c; // Update b to be the current Fibonacci number
        }
        printf("\n");
        printf("Fibonacci of %d is %lld\n", n, c); // Print the nth Fibonacci number
        return;
    }

}