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
} Game;