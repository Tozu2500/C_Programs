#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct to hold the dynamic array of individual strings
typedef struct {
    char **data;    // Array of string pointers
    int size;       // Number of string currently in the struct
    int capacity;   // Allocated memory capacity
} StringArray;

StringArray *createArray(int capacity);
void addString(StringArray *arr, const char *str);
void removeString(StringArray *arr, int index);
void printArray(const StringArray *arr);
void freeArray(StringArray *arr);

int main() {
    StringArray *arr = createArray(2);  // Starting with a capacity of 2

    addString(arr, "Hello");
    addString(arr, "World");
    addString(arr, "Memory");
    addString(arr, "Management");

    printf("Array contents:\n");
    printArray(arr);

    printf("\nRemoving index 1...\n");
    removeString(arr, 1);

    printf("Array after removal:\n");
    printArray(arr);

    freeArray(arr);  // Freeing all allocated memory

    return 0;
}

// Creating a new StringArray struct with the given capacity
StringArray *createArray(int capacity) {
    StringArray *arr = malloc(sizeof(StringArray));
    if (!arr) {
        perror("Failed to allocate memory for array structure");
        exit(EXIT_FAILURE);
    }
    arr->data = calloc(capacity, sizeof(char *));  // Initializing to NULL
    if (!arr->data) {
        perror("Failed to allocate memory for string pointers");
        free(arr);
        exit(EXIT_FAILURE);
    }
    arr->size = 0;
    arr->capacity = capacity;
    return arr;
}

// Adding a string into the dynamic array
void addString(StringArray *arr, const char *str) {
    if (arr->size >= arr->capacity) {
        // Need more space -> doubling capacity
        arr->capacity *= 2;
        char **newData = realloc(arr->data, arr->capacity * sizeof(char *));
        if (!newData) {
            perror("Failed to reallocate memory");
            freeArray(arr);
            exit(EXIT_FAILURE);
        }
        arr->data = newData;
    }

    // Deep copying the string (malloc new memory and copy contents)
    arr->data[arr->size] = malloc(strlen(str) + 1);
    if (!arr->data[arr->size]) {
        perror("Failed to allocate memory for a string");
        freeArray(arr);
        exit(EXIT_FAILURE);
    }
    strcpy(arr->data[arr->size], str);
    arr->size++;
}

// Removing a string at a given index
void removeString(StringArray *arr, int index) {
    if (index < 0 || index >= arr->size) {
        printf("Invalid index: %d\n", index);
        return;
    }

    free(arr->data[index]);  // Free the memory of that specific string

    // Shift all the elements left
    for (int i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }

    arr->data[arr->size - 1] = NULL;  // Avoid dangling pointers
    arr->size--;
}

// Print all of the strings
void printArray(const StringArray *arr) {
    printf("Size: %d, Capacity: %d\n", arr->size, arr->capacity);
    for (int i = 0; i < arr->size; i++) {
        printf("[%d] %s\n", i, arr->data[i]);
    }
}

// Free the entire array
void freeArray(StringArray *arr) {
    if (!arr) return;
    for (int i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}