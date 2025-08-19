#ifndef DISPLAY_H
#define DISPLAY_H

#include "game.h"

typedef struct {
    char** buffer;
    char** lastBuffer;
    int width;
    int height;
    int cursorX;
    int cursorY;
    int colorEnabled;
    int doubleBuffered;
} Display;

void initDisplay(Display* display, int width, int height);
void cleanupDisplay(Display* display);
void clearDisplay(Display* display);
void renderGame(Display* display, Game* game);
void renderBall(Display* display, Ball* ball);
void renderPaddles(Display* display, Paddle* left, Paddle* right);
void renderScore(Display* display, Score* score, int gameWidth);
void renderUI(Display* display, Game* game);
void renderMenu(Display* display, int selection);
void renderGameOver(Display* display, int winner);
void renderPauseScreen(Display* display);
void renderStats(Display* display, Score* score);
void flushDisplay(Display* display);
void setDisplayChar(Display* display, int x, int y, char c);
char getDisplayChar(Display* display, int x, int y);
void drawBorder(Display* display, int x1, int y1, int x2, int y2);
void drawHorizontalLine(Display* display, int x, int y, int length, char c);
void drawVerticalLine(Display* display, int x, int y, int length, char c);
void setCursor(Display* display, int x, int y);
void enableColor(Display* display);
void disableColor(Display* display);
void setDoubleBuffering(Display* display, int enabled);
void swapBuffers(Display* display);

#endif // DISPLAY_H