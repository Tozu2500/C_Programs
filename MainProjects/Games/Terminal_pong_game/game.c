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

void updateGame(Game* game) {
    if (game->gameState != GAME_RUNNING || game->paused) {
        return;
    }

    processGameLogic(game);
    updateBall(&game->ball, game->gameWidth, game->gameHeight);
    updatePaddle(&game->leftPaddle, game->gameHeight);
    updatePaddle(&game->rightPaddle, game->gameHeight);

    updateAI(&game->rightPaddle, getBallX(&game->ball), getBallY(&game->ball), game->gameHeight);

    CollisionInfo collisionInfo;

    if (checkBallPaddleCollision(&game->ball, &game->leftPaddle, &collisionInfo)) {
        resolveBallPaddleCollision(&game->ball, &game->leftPaddle, &collisionInfo);
        incrementRally(&game->score);
    }

    if (checkBallPaddleCollision(&game->ball, &game->rightPaddle, &collisionInfo)) {
        resolveBallPaddleCollision(&game->ball, &game->rightPaddle, &collisionInfo);
        incrementRally(&game->score);
    }

    if (checkBallWallCollision(&game->ball, game->gameWidth, game->gameHeight, &collisionInfo)) {
        resolveBallWallCollision(&game->ball, &collisionInfo);
    }

    if (getBallX(&game->ball) <= 0) {
        incrementRightScore(&game->score);
        resetBall(&game->ball, game->gameWidth, game->gameHeight);
        resetRally(&game->score);
    }

    if (getBallX(&game->ball) >= game->gameWidth - 1) {
        incrementLeftScore(&game->score);
        resetBall(&game->ball, game->gameWidth, game->gameHeight);
        resetRally(&game->score);
    }

    checkGameEnd(game);
}

void resetGame(Game* game) {
    resetBall(&game->ball, game->gameWidth, game->gameHeight);
    game->leftPaddle.y = game->gameHeight / 2 - 2;
    game->rightPaddle.y = game->gameHeight / 2 - 2;
    resetScore(&game->score);
    game->gameState = GAME_RUNNING;
    game->paused = 0;
    game->winner = 0;
}

void pauseGame(Game* game) {
    game->paused = 1;
    game->gameState = GAME_PAUSED;
}

void resumeGame(Game* game) {
    game->paused = 0;
    game->gameState = GAME_RUNNING;
}

void setGameSpeed(Game* game, int speed) {
    game->gameSpeed = clamp(speed, 1, 100);
    setBallSpeed(&game->ball, game->gameSpeed);
}

void setDifficulty(Game* game, int difficulty) {
    game->difficulty = clamp(difficulty, EASY_DIFFICULTY, HARD_DIFFICULTY);
    game->rightPaddle.aiDifficulty = game->difficulty;

    switch (difficulty) {
        case EASY_DIFFICULTY:
            game->rightPaddle.aiSpeed = 2;
            break;
        case MEDIUM_DIFFICULTY:
            game->rightPaddle.aiSpeed = 4;
            break;
        case HARD_DIFFICULTY:
            game->rightPaddle.aiSpeed = 6;
            break;
    }
}

void toggleSound(Game* game) {
    game->soundEnabled = !game->soundEnabled;
}

int isGameRunning(Game* game) {
    return game->running && game->gameState == GAME_RUNNING;
}

int isGamePaused(Game* game) {
    return game->paused;
}

int getGameWinner(Game* game) {
    return game->winner;
}

void checkGameEnd(Game* game) {
    if (getLeftScore(&game->score) >= WINNING_SCORE) {
        game->winner = PLAYER_LEFT;
        game->gameState = GAME_ENDED;
        game->running = 0;
    } else if (getRightScore(&game->score) >= WINNING_SCORE) {
        game->winner = PLAYER_RIGHT;
        game->gameState = GAME_ENDED;
        game->running = 0;
    }
}

void processGameLogic(Game* game) {
    if (game->ball.bounceCount > game->ball.maxBounces / 2) {
        increaseBallSpeed(&game->ball);
    }

    float ballSpeed = absFloat(game->ball.velocityX) + absFloat(game->ball.velocityY);
    if (ballSpeed > 10.0f) {
        game->ball.velocityX *= 0.95f;
        game->ball.velocityY *= 0.95f;
    }

    if (getCurrentRally(&game->score) > 20) {
        if (game->leftPaddle.height > 2) {
            setPaddleHeight(&game->leftPaddle, game->leftPaddle.height - 1);
        }

        if (game->rightPaddle.height > 2) {
            setPaddleHeight(&game->rightPaddle, game->rightPaddle.height - 1);
        }
    }
}