#ifndef UTILS_H
#define UTILS_H

#include <time.h>

typedef struct {
    clock_t startTime;
    clock_t lastTime;
    float deltaTime;
    float totalTime;
    int frameCount;
    float fps;
    int targetFPS;
} Timer;

void initTimer(Timer* timer, int targetFPS);
void updateTimer(Timer* timer);
float getDeltaTime(Timer* timer);
float getTotalTime(Timer* timer);
int shouldUpdate(Timer* timer);
void sleepMs(int milliseconds);
void waitForFrame(Timer* timer);
float getFPS(Timer* timer);
void setTargetFPS(Timer* timer, int fps);

int randomInt(int min, int max);
float randomFloat(float min, float max);
void seedRandom(void);
int clamp(int value, int min, int max);
float clampFloat(float value, float min, float max);
float lerp(float start, float end, float t);
int sign(float value);
float absFloat(float value);
int absInt(int value);
float minFloat(float a, float b);
float maxFloat(float a, float b);
int minInt(int a, int b);
int maxInt(int a, int b);

char* formatTime(float seconds);
char* formatScore(int score);
void copyString(char* dest, const char* src, int maxLen);
int stringLength(const char* str);
int stringCompare(const char* str1, const char* str2);
void intToString(int value, char* buffer, int bufferSize);
void floatToString(float value, char* buffer, int bufferSize, int precision);

#endif // UTILS_H