#include "paddle.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

void initPaddle(Paddle* paddle, int x, int y, int height, int player) {
    paddle->x = x;
    paddle->y = y;
    paddle->height = height;
    paddle->speed = 3;
    paddle->player = player;
    paddle->score = 0;
    paddle->aiEnabled = 0;
    paddle->aiSpeed = 3;
    paddle->aiDifficulty = 2;
    paddle->aiPosition = (float)y;
}

void updatePaddle(Paddle* paddle, int gameHeight) {
    checkPaddleBounds(paddle, gameHeight);
    smoothPaddleMovement(paddle);
}

void movePaddleUp(Paddle* paddle) {
    paddle->y -= paddle->speed;
}

void movePaddleDown(Paddle* paddle) {
    paddle->y += paddle->speed;
}

void setPaddleSpeed(Paddle* paddle, int speed) {
    paddle->speed = clamp(speed, 1, 10);
}

void setPaddleHeight(Paddle* paddle, int height) {
    paddle->height = clamp(height, 2, 8);
}

void enableAI(Paddle* paddle, int difficulty) {
    paddle->aiEnabled = 1;
    paddle->aiDifficulty = clamp(difficulty, 1, 3);

    switch (paddle->aiDifficulty) {
        case 1:
            paddle->aiSpeed = 2;
            break;
        case 2:
            paddle->aiSpeed = 3;
            break;
        case 3:
            paddle->aiSpeed = 4;
            break;
    }
}

int isPaddleAI(Paddle* paddle) {
    return paddle->aiEnabled;
}

int getPaddleTop(Paddle* paddle) {
    return paddle->y;
}

int getPaddleBottom(Paddle* paddle) {
    return paddle->y + paddle->height - 1;
}

int getPaddleX(Paddle* paddle) {
    return paddle->x;
}

void checkPaddleBounds(Paddle* paddle, int gameHeight) {
    if (paddle->y < 1) {
        paddle->y = 1;
        paddle->aiPosition = (float)paddle->y;
    }

    if (paddle->y + paddle->height >= gameHeight - 1) {
        paddle->y = gameHeight - paddle->height - 1;
        paddle->aiPosition = (float)paddle->y;
    }
}

void smoothPaddleMovement(Paddle* paddle) {
    if (paddle->aiEnabled) {
        float smoothing = 0.8f;
        paddle->aiPosition = lerp(paddle->aiPosition, (float)paddle->y, smoothing);
    }
}

void disableAI(Paddle* paddle) {
    paddle->aiEnabled = 0;
}

void updateAI(Paddle* paddle, int ballX, int ballY, int gameHeight) {
    if (!paddle->aiEnabled) {
        return;
    }

    int paddleCenter = paddle->y + paddle->height / 2;
    int targetY = ballY;

    float difficulty_factor = 1.0f;
    switch (paddle->aiDifficulty) {
        case 1:
            difficulty_factor = 0.6f;
            targetY += randomInt(-2, 2);
            break;
        case 2:
            difficulty_factor = 0.8f;
            targetY += randomInt(-1, 1);
            break;
        case 3:
            difficulty_factor = 1.0f;
            break;
    }

    int distance = abs(targetY - paddleCenter);
    if (distance > 1) {
        if (targetY < paddleCenter) {
            paddle->aiPosition -= paddle->aiSpeed * difficulty_factor;
        } else if (targetY > paddleCenter) {
            paddle->aiPosition += paddle->aiSpeed * difficulty_factor;
        }
    }

    paddle->y = (int)paddle->aiPosition;

    if (randomInt(1, 100) <= (4 - paddle->aiDifficulty)) {
        int error = randomInt(-1, 1);
        paddle->y += error;
    }
}