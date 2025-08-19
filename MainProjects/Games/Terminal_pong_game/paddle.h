#ifndef PADDLE_H
#define PADDLE_H

typedef struct {
    int x;
    int y;
    int height;
    int speed;
    int player;
    int score;
    int aiEnabled;
    int aiSpeed;
    int aiDifficulty;
    float aiPosition;
} Paddle;

void initPaddle(Paddle* paddle, int x, int y, int height, int player);
void updatePaddle(Paddle* paddle, int gameHeight);
void movePaddleUp(Paddle* paddle);
void movePaddleDown(Paddle* paddle);
void setPaddleSpeed(Paddle* paddle, int speed);
void setPaddleHeight(Paddle* paddle, int height);
void enableAI(Paddle* paddle, int difficulty);
void disableAI(Paddle* paddle);
void updateAI(Paddle* paddle, int ballX, int ballY, int gameHeight);
int isPaddleAI(Paddle* paddle);
int getPaddleTop(Paddle* paddle);
int getPaddleBottom(Paddle* paddle);
int getPaddleX(Paddle* paddle);
void checkPaddleBounds(Paddle* paddle, int gameHeight);
void smoothPaddleMovement(Paddle* paddle);

#endif // PADDLE_H