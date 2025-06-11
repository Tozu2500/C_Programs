#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int number, guess, attempts;
    char playAgain;

    do {
        // Seed the random number generator
        srand((unsigned int) time(NULL));
        number = rand() % 100 + 1;
        attempts = 0;

        printf("\nWelcome to the Number Guessing Game!\n");
        printf("I'm thinking of add number between 1 and 100. Can you guess it?\n");

        do {
            printf("Enter your guess: ");
            if (scanf("%d", &guess) != 1) {
                while (getchar() != '\n');
                printf("Invalid input. Please enter an integer.\n");
                continue;
            }

            attempts++;

            if (guess < number) {
                printf("Too low! Try again.\n");
            } else if (guess > number) {
                printf("Too high! Try again.\n");
            } else {
                printf("Congratulations! You guessed the number in %d attempts.\n", attempts);
            }
        } while (guess != number);

        printf("\nDo you want to play again? (y/n): ");
        while (getchar() != '\n'); // Clear newline left by previous input
        playAgain = getchar();

    } while (playAgain == 'y' || playAgain == 'Y');

    printf("Thank you for playing! Goodbye.\n");
    return 0;
}
