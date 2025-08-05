#include <stdio.h>
#include <time.h>

// Selection sort - O(n2) time, O(1) space
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            int temp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = temp;
        }
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
    selectionSort(arr, n);
    clock_t end = clock();

    printf("Sorted array: ");
    printArray(arr, n);
    printf("Time taken: %f seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    return 0;
}