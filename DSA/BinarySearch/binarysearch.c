#include <stdio.h>

int binarySearch(int arr[], int size, int target);

int main() {
    int arr[] = {2, 3, 4, 10, 40, 60, 20, 30, 50, 70, 80, 90, 33, 5234, 2311, 8553};
    int size = sizeof(arr) / sizeof(arr[0]);
    int target = 50;

    int result = binarySearch(arr, size, target);
    if (result != -1) {
        printf("Element found at index: %d\n", result);
    } else {
        printf("Element not found in the array.\n");
    }
    return 0;
}

int binarySearch(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        // Check it target is present at mid
        if (arr[mid] == target) {
            return mid; // Target found
        }
        // If target is greater, ignore left half
        else if (arr[mid] < target) {
            left = mid + 1;
        }
        // If target is smaller, ignore the right half
        else {
            right = mid - 1;
        }
    }
    return -1; // Target not found
}