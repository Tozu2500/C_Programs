#include "game.h"

// Array of guessable words for the game
static const char* words[WORD_COUNT] = {
    "COMPUTER", "PROGRAMMING", "CHALLENGE", "FUNCTION", "VARIABLE",
    "ALGORITHM", "DATABASE", "NETWORK", "SOFTWARE", "HARDWARE"
};

// Initialize game
void initGame(GameState *game) {
    strcpy(game->word, selectRandomWord());

    // Initialize guessed word with underscores
    for (int i = 0; i < strlen(game->word); i++) {
        game->guessed[i] = '_';
    }
    game->guessed[strlen(game->word)] = '\0';

    // Clear wrong letters
    memset(game->wrong_letters, 0, sizeof(game->wrong_letters));

    game->wrong_count = 0;
    game->guesses_left = MAX_GUESSES;
    game->game_won = 0;
}

// Select a random word from the list of the ten possible ones
char* selectRandomWord(void) {
    static char selected_word[MAX_WORD_LENGTH];
    int index = rand() % WORD_COUNT;
    strcpy(selected_word, words[index]);
    return selected_word;
}

// Display the current state of the guessed word
void displayWord(const GameState *game) {
    printf("Word: ");
    for (int i = 0; i < strlen(game->guessed); i++) {
        printf("%c", game->guessed[i]);
    }
    printf("\n");
}

// Display wrong letters
void displayWrongLetters(const GameState *game) {
    if (game->wrong_count > 0) {
        printf("Wrong letters: ");
        for (int i = 0; i < game->wrong_count; i++) {
            printf("%c", game->wrong_letters[i]);
        }
        printf("\n");
    }
}

// Display hangman ASCII art based on wrong guesses
void displayHangman(int wrong_count) {
    printf("\n  +---+\n");
    printf("  |   |\n");
    
    switch (wrong_count) {
        case 0:
            printf("  |    \n");
            printf("  |    \n");
            printf("  |    \n");
            printf("  |    \n");
            break;
        case 1:
            printf("  |   O\n");
            printf("  |    \n");
            printf("  |    \n");
            printf("  |    \n");
            break;
        case 2:
            printf("  |   O\n");
            printf("  |   |\n");
            printf("  |    \n");
            printf("  |    \n");
            break;
        case 3:
            printf("  |   O\n");
            printf("  |  /|\n");
            printf("  |    \n");
            printf("  |    \n");
            break;
        case 4:
            printf("  |   O\n");
            printf("  |  /|\\\n");
            printf("  |    \n");
            printf("  |    \n");
            break;
        case 5:
            printf("  |   O\n");
            printf("  |  /|\\\n");
            printf("  |  / \n");
            printf("  |    \n");
            break;
        case 6:
            printf("  |   O\n");
            printf("  |  /|\\\n");
            printf("  |  / \\\n");
            printf("  |    \n");
            break;
    }
    printf("__|__\n\n");
}

// Guess and update the game state
int makeGuess(GameState *game, char guess) {
    guess = toupper(guess);
    int found = 0;

    // Check if letter is in the word
    for (int i = 0; i < strlen(game->word); i++) {
        if (game->word[i] == guess) {
            game->guessed[i] = guess;
            found = 1;
        }
    }

    // If letter is not found, add it to wrong ones
    if (!found) {
        // Check if this letter is already tried
        for (int i = 0; i < game->wrong_count; i++) {
            if (game->wrong_letters[i] == guess) {
                return 0; // Already tried
            }
        }
        game->wrong_letters[game->wrong_count] = guess;
        game->wrong_count++;
        game->guesses_left--;
    }

    return found;
}

// Check games win condition
int isGameWon(const GameState *game) {
    return strcmp(game->word, game->guessed) == 0;
}

// Main game loop
void playGame(void) {
    GameState game;
    char guess;

    initGame(&game);

    printf("\n Word Guessing Game \n");
    printf("Guess the programming related word! You have %d wrong guesses allowed.\n\n", MAX_GUESSES);

    while (game.guesses_left > 0 && !isGameWon(&game)) {
        displayHangman(game.wrong_count);
        displayWord(&game);
        displayWrongLetters(&game);
        printf("Guesses remaining: %d\n", game.guesses_left);

        printf("\n Enter your guess (single letter): ");
        scanf(" %c", &guess);

        if (!isalpha(guess)) {
            printf("Please enter a valid letter!\n");
            continue;
        }

        int result = makeGuess(&game, guess);

        if (result) {
            printf("Good guess! That letter is in the word\n");
        } else {
            printf("Sorry, that letter is not in the word!\n");
        }
        printf("\n");
    }

    // Final result
    displayHangman(game.wrong_count);

    if (isGameWon(&game)) {
        printf("Congratulations! You guessed the word: %s\n", game.word);
    } else {
        printf("Game Over! The correct word was %s\n", game.word);
    }
}

void displayMenu(void) {
    printf("\n ___ WORD GUESSING GAME ___ \n");
    printf("1. Play the game\n");
    printf("2. Exit program\n");
    printf("Choose an option: ");
}