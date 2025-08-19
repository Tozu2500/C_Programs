#ifndef BALL_H
#define BALL_H

typedef struct {
    float x;
    float y;
    float velocityX;
    float velocityY;
    int lastX;
    int lastY;
    int speed;
    int bounceCount;
    int maxBounces;
} Ball;

void initBall(Ball* ball, int gameWidth, int gameHeight);
void updateBall(Ball* ball, int gameWidth, int gameHeight);
void resetBall(Ball* ball, int gameWidth, int gameHeight);
void setBallSpeed(Ball* ball, int speed);
void reverseBallX(Ball* ball);
void reverseBallY(Ball* ball);
void increaseBallSpeed(Ball* ball);
void setBallDirection(Ball* ball, float dirX, float dirY);
int getBallX(Ball* ball);
int getBallY(Ball* ball);
int getBallLastX(Ball* ball);
int getBallLastY(Ball* ball);
void checkBallBounds(Ball* ball, int gameWidth, int gameHeight);
void applyBallPhysics(Ball* ball);

#endif // BALL_H