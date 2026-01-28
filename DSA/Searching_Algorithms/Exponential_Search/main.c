#include <stdio.h>

int binarySearch(int arr[], int left, int right, int target) {

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target)
            return mid;

        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

int exponentialSearch(int arr[], int size, int target) {

    if (arr[0] == target)
        return 0;

    int i = 1;

    while (i < size && arr[i] <= target) {
        i *= 2;
    }

    int left = i / 2;
    int right = (i < size) ? i : size - 1;

    return binarySearch(arr, left, right, target);
}

int main() {
    int arr[] = {2, 3, 4, 10, 40, 55, 70, 88, 99};
    int size = sizeof(arr) / sizeof(arr[0]);
    int target = 55;

    int result = exponentialSearch(arr, size, target);

    if (result != -1)
        printf("Element found at index %d\n", result);
    else
        printf("Element not found\n");

    return 0;
}
