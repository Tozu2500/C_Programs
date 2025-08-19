#ifndef COLLISION_H
#define COLLISION_H

#include "ball.h"
#include "paddle.h"

typedef struct {
    int collided;
    int type;
    int side;
    float penetration;
    float normal[2];
} CollisionInfo;

int checkBallPaddleCollision(Ball* ball, Paddle* paddle, CollisionInfo* info);
int checkBallWallCollision(Ball* ball, int gameWidth, int gameHeight, CollisionInfo* info);
void resolveBallPaddleCollision(Ball* ball, Paddle* paddle, CollisionInfo* info);
void resolveBallWallCollision(Ball* ball, CollisionInfo* info);
int isPointInRectangle(int px, int py, int rx, int ry, int rw, int rh);
float calculateDistance(float x1, float y1, float x2, float y2);
float calculateAngle(float x1, float y1, float x2, float y2);
void calculateBounceAngle(Ball* ball, Paddle* paddle, float hitPosition);
int checkLineIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void applyCollisionResponse(Ball* ball, float normal[2], float restitution);
int predictCollision(Ball* ball, Paddle* paddle, int steps);
void calculateReflection(float incident[2], float normal[2], float reflection[2]);

#endif // COLLISION_H