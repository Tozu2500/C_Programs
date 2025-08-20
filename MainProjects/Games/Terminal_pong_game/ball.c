#include "ball.h"
#include "utils.h"
#include <math.>
#include <stdlib.h>

void initBall(Ball* ball, int gameWidth, int gameHeight) {
    ball->x = (float)(gameWidth / 2);
    ball->y = (float)(gameHeight / 2);
    ball->lastX = (int)ball->x;
    ball->lastY = (int)ball->y;
    ball->speed = 3;
    ball->bounceCount = 0;
    ball->maxBounces = 50;

    float angle = randomFloat(-0.5f, 0.5f);
    int direction = (randomInt(0, 1) == 0) ? -1 : 1;

    ball->velocityX = (float)direction * 2.0f;
    ball->velocityY = angle;
}

void updateBall(Ball* ball, int gameWidth, int gameHeight) {
    ball->lastX = (int)ball->x;
    ball->lastY = (int)ball->y;

    applyBallPhysics(ball);

    ball->x += ball->velocityX;
    ball->y += ball->velocityY;

    checkBallBounds(ball, gameWidth, gameHeight);
}

void resetBall(Ball* ball, int gameWidth, int gameHeight) {
    ball->x = (float)(gameWidth / 2);
    ball->y = (float)(gameHeight / 2);
    ball->lastX = (int)ball->x;
    ball->lastY = (int)ball->y;
    ball->bounceCount = 0;

    float angle = randomFloat(-0.5f, 0.5f);
    int direction = (randomInt(0, 1) == 0) ? -1 : 1;

    ball->velocityX = (float)direction * 2.0f;
    ball->velocityY = angle;
}

void setBallSpeed(Ball* ball, int speed) {
    ball->speed = clamp(speed, 1, 10);

    float currentSpeed = sqrtf(ball->velocityX * ball->velocityX + ball->velocityY * ball->velocityY);
    if (currentSpeed > 0) {
        float ratio = (float)ball->speed / currentSpeed;
        ball->velocityX *= ratio;
        ball->velocityY *= ratio;
    }
}

void reverseBallX(Ball* ball) {
    ball->velocityX = -ball->velocityX;
    ball->bounceCount++;
}

void reverseBallY(Ball* ball) {
    ball->velocityY = -ball->velocityY;
    ball->bounceCount++;
}

void increaseBallSpeed(Ball* ball) {
    float speedIncrease = 1.05f;
    ball->velocityX *= speedIncrease;
    ball->velocityY *= speedIncrease;

    float maxSpeed = 8.0f;
    if (absFloat(ball->velocityX) > maxSpeed) {
        ball->velocityX = (ball->velocityX > 0) ? maxSpeed : -maxSpeed;
    }

    if (absFloat(ball->velocityY) > maxSpeed) {
        ball->velocityY = (ball->velocityY > 0) ? maxSpeed : -maxSpeed;
    }
}

void setBallDirection(Ball* ball, float dirX, float dirY) {
    float length = sqrtf(dirX * dirX + dirY * dirY);
    if (length > 0) {
        ball->velocityX = (dirX / length) * (float)ball->speed;
        ball->velocityY = (dirY / length) * (float)ball->speed;
    }
}

int getBallX(Ball* ball) {
    return (int)ball->x;
}

int getBallY(Ball* ball) {
    return (int)ball->y;
}

int getBallLastX(Ball* ball) {
    return ball->lastX;
}

int getBallLastY(Ball* ball) {
    return ball->lastY;
}

void checkBallBounds(Ball* ball, int gameWidth, int gameHeight) {
    if (ball->y <= 1) {
        ball->y = 1;
        reverseBallY(ball);
    }

    if (ball->y >= gameHeight - 2) {
        ball->y = (float)(gameHeight - 2);
        reverseBallY(ball);
    }

    if (ball->x < 0) {
        ball->x = 0;
    }

    if (ball->x >= gameWidth) {
        ball->x = (float)(gameWidth - 1);
    }
}

void applyBallPhysics(Ball* ball) {
    float friction = 0.999f;
    ball->velocityX *= friction;
    ball->velocityY *= friction;

    float minVelocity = 0.1f;

    if (absFloat(ball->velocityX) < minVelocity) {
        ball->velocityX = (ball->velocityX >= 0) ? minVelocity : -minVelocity;
    }

    if (absFloat(ball->velocityY) < minVelocity && absFloat(ball->velocityY) > 0) {
        ball->velocityY = (ball->velocityY >= 0) ? minVelocity : -minVelocity;
    }


    if (ball->bounceCount > ball->maxBounces) {
        ball->velocityY += randomFloat(-0.1f, 0.1f);
        ball->bounceCount = 0;
    }
}