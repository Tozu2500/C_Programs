#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototype for linear search
int linearSearch(unsigned long arr[], int n, unsigned long target);

int main() {
    // Create a large array of random unsigned long integers
    unsigned long arr[100000];
    srand((unsigned long) time(NULL)); // Seed the random number generator

    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        arr[i] = rand() % 1000000; // Random numbers between 0 and 99999
    }

    // Define the target value to search for
    unsigned long target = arr[rand() % (sizeof(arr) / sizeof(arr[0]))]; // Randomly selecting a target from the array
    printf("We have an array of 10000 random unsigned long integers.\n");
    printf("The numbers can be between 0 and 99999.\n");
    printf("The target value (randomly selected from the array) that we will search for is: %.12lf\n", target);

    // Save current time before starting the search
    time_t start, end;
    time(&start); // Start time
    printf("Start time: %s", ctime(&start)); // Print the start time

    // Call the linear search function
    int index = linearSearch(arr, sizeof(arr) / sizeof(arr[0]), target);

    // Check if the target was found and print the result
    if (index != -1) {
        printf("Target value %lu found at index %d.\n", target, index);
    } else {
        printf("Target value %lu not found in the array.\n", target);
    }

    // Save current time after finishing the search
    time(&end); // End time
    printf("End time: %s", ctime(&end)); // End time print

    // Calculate and print the total time it took for the search to complete
    double elapsedTime = difftime(end, start);
    printf("Elapsed time: %.9f seconds\n", elapsedTime);

    return 0;
}

int linearSearch(unsigned long arr[], int n, unsigned long target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            return i; // Return the index if the target is found
        }
    }
    return -1; // Otherwise, return -1 indicating that the target was not found
}