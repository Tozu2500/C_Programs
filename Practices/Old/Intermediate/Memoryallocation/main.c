#include <stdio.h>
#include <stdlib.h>

struct List {
    int *data;        // Pointer for the array of data
    int numItems;     // Current number of items on the list
    int size;         // Allocated capacity
};

void addToList(struct List *myList, int item) {
    // Resizing, if full
    if (myList->numItems == myList->size) {
        int newSize = myList->size + 10;
        int *temp = realloc(myList->data, newSize * sizeof(int));
        if (temp == NULL) {
            printf("Memory resizing failed!\n");
            return;
        }
        myList->data = temp;
        myList->size = newSize;
    }

    // Adding an item
    myList->data[myList->numItems] = item;
    myList->numItems++;
}

int main() {
    struct List myList;
    myList.numItems = 0;
    myList.size = 10;
    myList.data = malloc(myList.size * sizeof(int));

    if (myList.data == NULL) {
        printf("Initial memory allocation failed!\n");
        return 1;
    }

    // Adding 44 items to the list
    for (int i = 0; i < 44; i++) {
        addToList(&myList, i + 1);
    }

    // Print the list
    for (int j = 0; j < myList.numItems; j++) {
        printf("%d ", myList.data[j]);
        printf("\n");
    }
    

    // Free the memory
    free(myList.data);
    myList.data = NULL;

    return 0;
}