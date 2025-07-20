#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define MAX_WIDTH 80
#define MAX_HEIGHT 24
#define MAX_SNAKE_LENGTH 1000
#define SAVE_FILE "snake_save.dat"
#define STATS_FILE "snake_stats.dat"

typedef enum {
    DIR_UP = 0;
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    Direction dir;
} Snake;

typedef struct {
    int width, height;
    Point food;
    Snake snake;
    int score;
    int game_over;
    int paused;
} Game;

typedef struct {
    int games_played;
    int total_score;
    int high_score;
    int total_food_eaten;
    time_t last_played;
} Stats;

// Function protos
// Game functions
void init_game(Game *game, int width, int height);
void update_game(Game *game);
void render_game(const Game *game);
int check_collision(const Game *game);
void place_food(Game *game);
void grow_snake(Snake *snake);
void move_snake(Snake *snake);

// Input functions
void setup_terminal(void);
void restore_terminal(void);
int kbhit(void);
char getch(void);

// Save & Load functions
int save_game(const Game *game);
int load_game(Game *game);
void save_stats(const Stats *stats);
Stats load_stats(void);
void update_stats(Stats *stats const Game *game);

// Utils
void clear_screen(void);
void move_cursor(int x, int y);
void hide_cursor(void);
void show_cursor(void);

#endif // SNAKE_H