#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes
void playGame(int difficulty);
void showStats(int games[], int size);
int getValidInput(int min, int max);

int main() {

    int choice = 0;
    int gameStats[3] = {0, 0, 0}; // Easy, medium, hard

    // Seed random number gen
    srand(time(NULL));

    printf("--- Welcome to the number guessing game! ---\n");

    do {
        printf("Menu\n");
        printf("1. Play on Easy mode (1 - 10)\n");
        printf("2. Play on Medium mode (1 - 50)\n");
        printf("3. Play on hard mode (1 - 100)\n");
        printf("4. View your statistics\n");
        printf("5. Exit\n");
        printf("Enter your choice: \n");

        choice = getValidInput(1, 5);

        switch(choice) {
            case 1:
                playGame(10);
                gameStats[0]++;
                break;
            case 2:
                playGame(50);
                gameStats[1]++;
                break;
            case 3:
                playGame(100);
                gameStats[2]++;
                break;
            case 4:
                showStats(gameStats, 3);
                break;
            case 5:
                printf("Thanks for playing! Bye bye!\n");
                break;
        }
        printf("\n");
    } while (choice != 5);

    return 0;
}

void playGame(int range) {
    int target = rand() % range + 1;
    int guess = 0;
    int attempts = 0;

    int maxAttempts = (range <= 10) ? 4 : (range <= 50) ? 7 : 10;

    printf("\n I am thinking of a number between 1 and %d\n", range);
    printf("You have %d attempts to guess it!\n", maxAttempts);

    do {
        printf("Enter your guess: ");
        guess = getValidInput(1, range);
        attempts++;

        if (guess == target) {
            printf("Congratulations! You got the number right in %d attempts!\n", attempts);
            return;
        } else if (guess < target) {
            printf("Too low! ");
        } else {
            printf("Too high! ");
        }

        printf("Attempts remaining: %d\n", maxAttempts - attempts);

    } while (attempts < maxAttempts);

    printf("Game over! The correct number was %d\n", target);
}

void showStats(int games[], int size) {
    char *levels[] = {"Easy", "Medium", "Hard"};
    int total = 0;

    printf("\n--- Game statistics ---\n");
    for (int i = 0; i < size; i++) {
        printf("%s games played: %d\n", levels[i], games[i]);
        total += games[i];
    }
    printf("Total games played: %d\n", total);
}

int getValidInput(int min, int max) {
    int input;
    while (scanf("%d", &input) != 1 || input < min || input > max) {
        printf("Please enter a number between %d and %d: ", min, max);
        while(getchar() != '\n'); // Clearing the input buffer
    }
    return input;
}