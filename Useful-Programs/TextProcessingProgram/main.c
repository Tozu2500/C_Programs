#include "textproc.h"

int main() {
    int choice;

    printf("Text Processing Program\n\n");

    while (1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline buffer

        if (choice == 6) {
            printf("Goodbye!\n");
            break;
        }

        handleMenuChoice(choice);
        printf("\n");
    }

    return 0;
}