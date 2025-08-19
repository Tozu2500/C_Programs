#include "game.h"
#include "collision.h"
#include "utils.h"
#include <stdlib.h>

void initGame(Game* game, int width, int height) {
    game->gameWidth = clamp(width, MIN_GAME_WIDTH, MAX_GAME_WIDTH);
    game->gameHeight = clamp(height, MIN_GAME_HEIGHT, MAX_GAME_HEIGHT);
    game->gameState = GAME_RUNNING;
    game->running = 1;
    game->paused = 0;
    game->winner = 0;
    game->gameSpeed = 50;
    game->difficulty = MEDIUM_DIFFICULTY;
    game->soundEnabled = 1;

    initBall(&game->ball, game->gameWidth, game->gameHeight);
    initPaddle(&game->leftPaddle, 2, game->gameHeight / 2 - 2, 4, PLAYER_LEFT);
    initPaddle(&game->rightPaddle, game->gameWidth - 3, game->gameHeight / 2 - 2, 4, PLAYER_RIGHT);
    initScore(&game->score);

    enableAI(&game->rightPaddle, game->difficulty);
}