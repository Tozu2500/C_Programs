#include <stdio.h>

void swap(int *a, int *b) {
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}

int main() {
    int x = 10;
    int y = 20;
    printf("Before swap: x: %d y: %d\n", x, y);
    swap(&x, &y);
    printf("After swap: x: %d y: %d", x, y);

    return 0;
}