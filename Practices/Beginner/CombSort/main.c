#include <stdio.h>
#include <stdbool.h>

void combSort(int arr[], int n) {
    int gap = n;
    const float shrink = 1.3;
    bool swapped = true;

    while (gap > 1 || swapped) {
        // Update gap for next pass
        gap = (int)(gap / shrink);
        if (gap < 1) gap = 1;

        swapped = false;

        // Compare all elements with current gap
        for (int i = 0; i + gap < n; i++) {
            if (arr[i] > arr[i + gap]) {
                int temp = arr[i];
                arr[i] = arr[i + gap];
                arr[i + gap] = temp;
                swapped = true;
            }
        }
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    int data[] = {64, 34, 25, 12, 22, 11, 90};
    int size = sizeof(data) / sizeof(data[0]);

    printf("Original array: ");
    printArray(data, size);

    combSort(data, size);

    printf("Sorted array:   ");
    printArray(data, size);

    return 0;
}
