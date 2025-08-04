#include <stdio.h>
#include <time.h>

// Insertion sort - O(n2) time, O(1) space
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original array: ");
    printArray(arr, n);

    clock_t start = clock();
    insertionSort(arr, n);
    clock_t end = clock();

    printf("Sorted array: ");
    printArray(arr, n);
    printf("Time taken: %f seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    return 0;
}