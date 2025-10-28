#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Iterative interpolation search
int interpolation_search_iterative(const int arr[], int n, int x)
{
    if (n <= 0 || !arr) return -1;

    int lo = 0, hi = n - 1;

    while (lo <= hi && x >= arr[lo] && x <= arr[hi]) {
        if (arr[hi] == arr[lo]) {
            return (arr[lo] == x) ? lo : -1;
        }

        double ratio = (double)(x - arr[lo]) / (double)(arr[hi] - arr[lo]);
        long long probe = lo + (long long)((hi - lo) * ratio);

        if (probe < lo) probe = lo;
        if (probe > hi) probe = hi;

        int pos = (int)probe;

        if (arr[pos] == x) return pos;
        if (arr[pos] < x) lo = pos + 1;
        else hi = pos - 1;
    }

    if (lo < n && arr[lo] == x) return lo;
    return -1;
}

// Recursive version of interpolation search
int interpolation_search_recursive_internal(const int arr[], int lo, int hi, int x)
{
    if (lo > hi || !arr) return -1;
    if (arr[hi] == arr[lo]) return (arr[lo] == x) ? lo : -1;
    if (x < arr[lo] || x > arr[hi]) return -1;

    double ratio = (double)(x - arr[lo]) / (double)(arr[hi] - arr[lo]);
    long long probe = lo + (long long)((hi - lo) * ratio);

    if (probe < lo) probe = lo;
    if (probe > hi) probe = hi;

    int pos = (int)probe;

    if (arr[pos] == x) return pos;
    if (arr[pos] < x) return interpolation_search_recursive_internal(arr, pos + 1, hi, x);
    return interpolation_search_recursive_internal(arr, lo, pos - 1, x);
}

int interpolation_search_recursive(const int arr[], int n, int x)
{
    if (n <= 0 || !arr) return -1;
    return interpolation_search_recursive_internal(arr, 0, n - 1, x);
}

void print_array(const int arr[], int n)
{
    printf("{");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i + 1 < n) printf(", ");
    }
    printf(" }");
}

int main() {
    int arr1[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    int targets1[] = {10, 55, 100, 35};
    printf("Array 1: ");
    print_array(arr1, n1);
    printf("\n");
    for (int i = 0; i < (int)(sizeof(targets1)/sizeof(targets1[0])); i++) {
        int t = targets1[i];
        int it = interpolation_search_iterative(arr1, n1, t);
        int rec = interpolation_search_recursive(arr1, n1, t);
        printf("Searching %d -> iterative: %d, recursive: %d\n", t, it, rec);
    }
    printf("\n");

    int arr2[] = {5, 5, 5, 6, 7, 7, 8, 9};
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    printf("Array 2: ");
    print_array(arr2, n2);
    printf("\n");
    printf("Searching 5 -> iterative: %d\n\n", interpolation_search_iterative(arr2, n2, 5));

    const int n3 = 100;
    int *arr3 = malloc(sizeof(int) * n3);
    if (!arr3) return 2;
    int base = 1000, step = 5;
    for (int i = 0; i < n3; i++) arr3[i] = base + i * step;
    printf("Array 3 (first 8 shown): ");
    print_array(arr3, 8);
    printf(", ...\n");
    int t3_present = base + 37 * step;
    int t3_end = base + (n3 - 1) * step;
    printf("Searching %d -> %d (expected 37)\n", t3_present, interpolation_search_iterative(arr3, n3, t3_present));
    printf("Searching %d -> %d (expected %d)\n\n", t3_end, interpolation_search_iterative(arr3, n3, t3_end), n3 - 1);
    free(arr3);

    srand((unsigned)time(NULL));
    int n4 = 50;
    int *arr4 = malloc(sizeof(int) * n4);
    if (!arr4) return 3;
    int cur = 0;
    for (int i = 0; i < n4; ++i) {
        cur += rand() % 10;
        arr4[i] = cur;
    }
    printf("Array 4 (first 12 shown): ");
    print_array(arr4, (n4 < 12) ? n4 : 12);
    if (n4 > 12) printf(", ...");
    printf("\n");
    int failures = 0;
    for (int i = 0; i < n4; i++) {
        int v = arr4[i];
        int idx = interpolation_search_iterative(arr4, n4, v);
        if (idx == -1 || arr4[idx] != v) {
            ++failures;
            printf("Failed to find %d (got %d)\n", v, idx);
        }
        printf("Random test: %s (%d failures)\n", failures == 0 ? "OK" : "SOME FAILURES", failures);
        free(arr4);

        return 0;
    }
}