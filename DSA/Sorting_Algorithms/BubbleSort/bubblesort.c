#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Function prototype for bubblesort
void bubbleSort(int arr[], int n);

int main() {
    int arr[10000];

    // Seed the array with random numbers
    srand((unsigned long) time(NULL));
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        arr[i] = rand() % 10000; // Random numbers between 0 and 999 fill the array
    }

    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Unsorted array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Save current time before sorting
    printf("Sorting the array using Bubble Sort...\n\n");
    
    // Save current time after sorting
    time_t start, end;
    time(&start); // Start time
    printf("Start time: %s", ctime(&start));
    bubbleSort(arr, n);
    time(&end); // End time
    printf("End time: %s", ctime(&end));
    double elapsedTime = difftime(end, start);
    printf("Elapsed time: %.6f seconds\n", elapsedTime);

    // Check if the array is sorted correctly, and then print the time taken
    printf("Checking if the array is sorted correctly...\n");
    printf("Start time was: %s", ctime(&start));
    printf("\n\nEnd time was: %s", ctime(&end));
    printf("\n\nElapsed time was: %.15lf seconds\n", elapsedTime);

    return 0;
}

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0; // Flag to check if a swap occurred
        for (int j = 0; j < n - i - 1; j++) {
            // Compare adjacent elements
            if (arr[j] > arr[j + 1]) {
                // Swap if they are in the wrong order
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped++; // Set the flag to indicate a swap occurred
            }
        }
        // If no two elements were swapped in the inner loop, then the array is sorted
        if (swapped == 0)
            break; // Array is sorted, exiting the loop
    }
}