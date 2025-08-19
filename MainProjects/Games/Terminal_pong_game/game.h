#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "ball.h"
#include "paddle.h"
#include "score.h"

typedef struct {
    Ball ball;
    Paddle leftPaddle;
    Paddle rightPaddle;
    Score score;
    int gameState;
    int gameWidth;
    int gameHeight;
    int running;
    int paused;
    int winner;
    int gameSpeed;
    int difficulty;
    int soundEnabled;
} Game;

void initGame(Game* game, int width, int height);
void updateGame(Game* game);
void resetGame(Game* game);
void pauseGame(Game* game);
void resumeGame(Game* game);
void setGameSpeed(Game* game, int speed);
void setDifficulty(Game* game, int difficulty);
void toggleSound(Game* game);
int isGameRunning(Game* game);
int isGamePaused(Game* game);
int getGameWinner(Game* game);
void checkGameEnd(Game* game);
void processGameLogic(Game* game);

#endif // GAME_H