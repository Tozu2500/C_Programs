#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void initTimer(Timer* timer, int targetFPS) {
    timer->startTime = clock();
    timer->lastTime = timer->startTime;
    timer->deltaTime = 0.0f;
    timer->totalTime = 0.0f;
    timer->frameCount = 0;
    timer->fps = 0.0f;
    timer->targetFPS = targetFPS;
}

void updateTimer(Timer* timer) {
    clock_t currentTime = clock();
    timer->deltaTime = (float)(currentTime - timer->lastTime) / CLOCKS_PER_SEC;
    timer->totalTime = (float)(currentTime - timer->startTime) / CLOCKS_PER_SEC;
    timer->lastTime = currentTime;
    timer->frameCount++;

    if (timer->frameCount % 60 == 0) {
        timer->fps = 60.0f / timer->totalTime * timer->frameCount;
    }
}

float getDeltaTime(Timer* timer) {
    return timer->deltaTime;
}

float getTotalTime(Timer* timer) {
    return timer->totalTime;
}

int shouldUpdate(Timer* timer) {
    float targetFrameTime = 1000.0f / timer->targetFPS;
    float currentFrameTime = timer->deltaTime * 1000.0f;
    return currentFrameTime >= targetFrameTime;
}

void sleepMs(int milliseconds) {
    Sleep(milliseconds);
}

void waitForFrame(Timer* timer) {
    float targetFrameTime = 1000.0f / timer->targetFPS;
    float currentFrameTime = timer->deltaTime * 1000.0f;

    if (currentFrameTime < targetFrameTime) {
        sleepMs((int)(targetFrameTime - currentFrameTime));
    }
}

float getFPS(Timer* timer) {
    return timer->fps;
}

void setTargetFPS(Timer* timer, int fps) {
    timer->targetFPS = clamp(fps, 1, 1000);
}

int randomInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

float randomFloat(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

void seedRandom(void) {
    srand((unsigned int)time(NULL));
}

int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float clampFloat(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float lerp(float start, float end, float t) {
    return start + t * (end - start);
}

int sign(float value) {
    if (value > 0) return 1;
    if (value < 0) return -1;
    return 0;
}

float absFloat(float value) {
    return (value < 0) ? -value : value;
}

int absInt(int value) {
    return (value < 0) ? -value : value;
}

float minFloat(float a, float b) {
    return (a < b) ? a : b;
}

float maxFloat(float a, float b) {
    return (a > b) ? a : b;
}

int minInt(int a, int b) {
    return (a < b) ? a : b;
}

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

char* formatTime(float seconds) {
    static char timeBuffer[32];
    int minutes = (int)(seconds / 60);
    int secs = (int)(seconds) % 60;
    sprintf(timeBuffer, "%02d:%02d", minutes, secs);
    return timeBuffer;
}

char* formatScore(int score) {
    static char scoreBuffer[16];
    sprintf(scoreBuffer, "%d", score);
    return scoreBuffer;
}

void copyString(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int stringLength(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

int stringCompare(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

void intToString(int value, char* buffer, int bufferSize) {
    sprintf(buffer, "%d", value);
}

void floatToString(float value, char* buffer, int bufferSize, int precision) {
    char format[16];
    sprintf(format, "%%.%df", precision);
    sprintf(buffer, format, value);
}