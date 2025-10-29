#include <stdio.h>

int main() {
    int arr[] = {4, 7, 1, 9, 3};
    int *p = arr;
    int max = *p;
    int min = *p;

    for (int i = 0; i < 5; i++) {
        if (*(p+i) > max) max = *(p+i);
        if (*(p+i) < min) min = *(p+i);
    }

    printf("Max = %d,  Min = %d\n", max, min);

    return 0;
}