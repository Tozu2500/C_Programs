#include "game.h"

int main(void) {
    int choice;

    srand(time(NULL));

    printf("Welcome to the word guessing game!\n");

    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                printf("Thanks for playing! Goodbye!\n");
                break;
            default:
                printf("Invalid choice!\n");
                break;
        }

        if (choice == 1) {
            printf("Would you like to play again? (1 = Yes, 2 = No): ");
            scanf("%d", &choice);
            if (choice == 1) {
                choice = 0; // Continue the loop
            } else {
                choice = 2; // Exit
            }
        }
    } while (choice != 2);

    return 0;
}