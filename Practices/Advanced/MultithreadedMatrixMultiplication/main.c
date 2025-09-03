#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 3

int A[SIZE][SIZE];
int B[SIZE][SIZE];
int C[SIZE][SIZE];

typedef struct {
    int row;
    int col;
} Data;

void* multiply(void *arg) {
    Data *d = arg;
    int sum = 0;
    for (int k = 0; k < SIZE; k++) {
        sum += A[d->row][k] * B[k][d->col];
    }
    C[d->row][d->col] = sum;
    free(d);
    return NULL;
}

int main() {
    printf("Enter matrixA (3x3):\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    printf("Enter matrix B (3x3):\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    pthread_t threads[SIZE*SIZE];
    int idx = 0;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Data *d = malloc(sizeof(Data));
            d->row = i;
            d->col = j;
            pthread_create(&threads[idx++], NULL, multiply, d);
        }
    }

    for (int i = 0; i < idx; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nResult Matrix:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}