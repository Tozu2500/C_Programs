#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

// Constants
#define MAX_SIZE 8
#define MIN_SIZE 4
#define MAX_SYMBOLS 32
#define MAX_NAME_LENGTH 50

typedef enum {
    EASY = 4,
    MEDIUM = 6,
    HARD = 8
} Difficulty;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    int moves;
    Difficulty difficulty;
} Player;

typedef struct {
    char symbol;
    int revealed;
    int matched;
} Card;

typedef struct {
    Card board[MAX_SIZE][MAX_SIZE];
    int size;
    int matched_pairs;
    int total_pairs;
    int moves;
    Player player;
} Game;

void init_game(Game* game, Difficulty diff);
void shuffle_cards(Game* game);
void display_board(Game* game);
void clear_screen(void);
void set_color(int color);
void reset_color(void);
int get_input(void);
void move_cursor(int x, int y);
int play_turn(Game* game);
int check_win(Game* game);
void show_menu(void);
void show_instructions(void);
void show_highscores(void);
void save_score(Player* player);
void load_scores(void);
int main_menu(void);

#endif //GAME_H