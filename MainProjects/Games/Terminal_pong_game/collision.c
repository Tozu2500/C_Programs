#include "collision.h"
#include "utils.h"
#include "types.h"
#include <math.h>
#include <stdlib.h>

int checkBallPaddleCollision(Ball* ball, Paddle* paddle, CollisionInfo* info) {
    int ballX = getBallX(ball);
    int ballY = getBallY(ball);

    int paddleLeft = getPaddleX(paddle);
    int paddleRight = paddleLeft;
    int paddleTop = getPaddleTop(paddle);
    int paddleBottom = getPaddleBottom(paddle);

    info->collided = 0;
    info->type = 0;
    info->side = 0;
    info->penetration = 0.0f;
    info->normal[0] = 0.0f;
    info->normal[1] = 0.0f;

    if (ballX >= paddleLeft - 1 && ballX <= paddleRight + 1 &&
        ballY >= paddleTop && ballY <= paddleBottom) {

            info->collided = 1;
            info->type = 1;

            if (paddle->player == PLAYER_LEFT) {
                info->side = 1;
                info->normal[0] = 1.0f;
                info->normal[1] = 0.0f;
            } else {
                info->side = 2;
                info->normal[0] = -1.0f;
                info->normal[1] = 0.0f;
            }

            info->penetration = 1.0f;
            return 1;
        }

    return 0;
}

int checkBallWallCollision(Ball* ball, int gameWidth, int gameHeight, CollisionInfo* info) {
    int ballX = getBallX(ball);
    int ballY = getBallY(ball);

    info->collided = 0;
    info->type = 0;
    info->side = 0;
    info->penetration = 0.0f;
    info->normal[0] = 0.0f;
    info->normal[1] = 0.0f;

    if (ballY <= 1) {
        info->collided = 1;
        info->type = 2;
        info->side = 1;
        info->normal[0] = 0.0f;
        info->normal[1] = 1.0f;
        info->penetration = 1.0f - ballY;
        return 1;
    }

    if (ballY >= gameHeight - 2) {
        info->collided = 1;
        info->type = 2;
        info->side = 2;
        info->normal[0] = 0.0f;
        info->normal[1] = -1.0f;
        info->penetration = ballY - (gameHeight - 2);
        return 1;
    }

    return 0;
}

void resolveBallPaddleCollision(Ball* ball, Paddle* paddle, CollisionInfo* info) {
    if (!info->collided) {
        return;
    }

    int paddleCenter = getPaddleTop(paddle) + paddle->height / 2;
    int ballY = getBallY(ball);
    int hitPosition = ballY - paddleCenter;

    calculateBounceAngle(ball, paddle, (float)hitPosition);

    reverseBallX(ball);

    float speedIncrease = 1.02f;
    ball->velocityX *= speedIncrease;
    ball->velocityY *= speedIncrease;

    float maxSpeed = 6.0f;
    if (absFloat(ball->velocityX) > maxSpeed) {
        ball->velocityX = (ball->velocityX > 0) ? maxSpeed : -maxSpeed;
    }

    if (absFloat(ball->velocityY) > maxSpeed) {
        ball->velocityY = (ball->velocityY > 0) ? maxSpeed : -maxSpeed;
    }

    if (paddle->player == PLAYER_LEFT) {
        ball->x = (float)(getPaddleX(paddle) + 1);
    } else {
        ball->x = (float)(getPaddleX(paddle) - 1);
    }
}

void resolveBallWallCollision(Ball* ball, CollisionInfo* info) {
    if (!info->collided) {
        return;
    }
    
    reverseBallY(ball);
    
    if (info->side == 1) {
        ball->y = 1.0f;
    } else {
        ball->y = (float)(info->side - 2);
    }
}

int isPointInRectangle(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px < rx + rw && py >= ry && py < ry + rh);
}

float calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

float calculateAngle(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return atan2f(dy, dx);
}

void calculateBounceAngle(Ball* ball, Paddle* paddle, float hitPosition) {
    float maxAngle = 0.75f;
    float normalizedHitPos = hitPosition / (paddle->height / 2.0f);
    normalizedHitPos = clampFloat(normalizedHitPos, -1.0f, 1.0f);

    float bounceAngle = normalizedHitPos * maxAngle;

    float speed = sqrtf(ball->velocityX * ball->velocityX + ball->velocityY * ball->velocityY);

    int direction = (paddle->player == PLAYER_LEFT) ? 1 : -1;

    ball->velocityX = direction * speed * cosf(bounceAngle);
    ball->velocityY = speed * sinf(bounceAngle);

    float minSpeed = 1.5f;
    if (absFloat(ball->velocityX) < minSpeed) {
        ball->velocityX = (ball->velocityX >= 0) ? minSpeed : -minSpeed;
    }
}

int checkLineIntersection(float x1, float y1, float x2, float y2,
                        float x3, float y3, float x4, float y4) {

    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (absFloat(denom) < 0.001f) {
        return 0;
    }

    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

void applyCollisionResponse(Ball* ball, float normal[2], float restitution) {
    float dotProduct = ball->velocityX * normal[0] + ball->velocityY * normal[1];

    ball->velocityX -= 2 * dotProduct * normal[0] * restitution;
    ball->velocityY -= 2 * dotProduct * normal[1] * restitution;
}

int predictCollision(Ball* ball, Paddle* paddle, int steps) {
    float tempX = ball->x;
    float tempY = ball->y;
    float tempVelX = ball->velocityX;
    float tempVelY = ball->velocityY;

    for (int i = 0; i < steps; i++) {
        tempX += tempVelX;
        tempY += tempVelY;

        int ballX = (int)tempX;
        int ballY = (int)tempY;

        if (ballX >= getPaddleX(paddle) - 1 && ballX <= getPaddleX(paddle) + 1 &&
            ballY >= getPaddleTop(paddle) && ballY <= getPaddleBottom(paddle)) {
            return 1;
        }
    }

    return 0;
}

void calculateReflection(float incident[2], float normal[2], float reflection[2]) {
    float dotProduct = incident[0] * normal[0] + incident[1] * normal[1];

    reflection[0] = incident[0] - 2 * dotProduct * normal[0];
    reflection[1] = incident[1] - 2 * dotProduct * normal[1];
}