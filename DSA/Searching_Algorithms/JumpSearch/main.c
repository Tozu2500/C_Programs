#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Function to perform Jump Search
int jumpSearch(int arr[], int n, int target) {
    if (n == 0) return -1; // Handle empty array
    
    int step = sqrt(n);  // Optimal block size to jump
    int prev = 0;

    // Finding the block where element is present
    while (arr[(step < n ? step : n) - 1] < target) {
        prev = step;
        step += sqrt(n);
        if (prev >= n)
            return -1; // Target not found
    }

    // Linear search within the found block
    for (int i = prev; i < step && i < n; i++) {
        if (arr[i] == target)
            return i; // Target found
    }

    return -1; // Target not found
}

int main() {
    int n, target;

    printf("Enter the number of elements in the sorted array: ");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Array size must be positive.\n");
        return 0;
    }

    int *arr = (int*)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d sorted elements (in ascending order):\n", n);
    for (int i = 0; i < n; i++) {
        printf("Element %d: ", i + 1);
        scanf("%d", &arr[i]);
    }

    printf("Enter the element to search for: ");
    scanf("%d", &target);

    int index = jumpSearch(arr, n, target);

    if (index != -1)
        printf("\nElement %d found at index %d.\n", target, index);
    else
        printf("\nElement %d not found in the array.\n", target);

    free(arr); // Free allocated memory
    return 0;
}
