#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 20
#define MAX_GUESSES 6
#define WORD_COUNT 10

// Structure for game state
typedef struct {
    char word[MAX_WORD_LENGTH];
    char guessed[MAX_WORD_LENGTH];
    char wrong_letters[26];
    int wrong_count;
    int guesses_left;
    int game_won;
} GameState;

// Function declarations
void initGame(GameState *game);
char* selectRandomWord(void);
void displayWord(const GameState *game);
void displayWrongLetters(const GameState *game);
void displayHangman(int wrong_count);
int makeGuess(GameState *game, char guess);
int isGameWon(const GameState *game);
void playGame(void);
void displayMenu(void);

#endif