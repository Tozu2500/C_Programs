#include "display.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void initDisplay(Display* display, int width, int height) {
    display->width = width;
    display->height = height;
    display->cursorX = 0;
    display->cursorY = 0;
    display->colorEnabled = 1;
    display->doubleBuffered = 1;

    display->buffer = (char**)malloc(height * sizeof(char*));
    display->lastBuffer = (char**)malloc(height * sizeof(char*));

    for (int i = 0; i < height; i++) {
        display->buffer[i] = (char*)malloc((width + 1) * sizeof(char));
        display->lastBuffer[i] = (char*)malloc((width + 1) * sizeof(char));
        memset(display->buffer[i], EMPTY_CHAR, width);
        memset(display->lastBuffer[i], EMPTY_CHAR, width);
        display->buffer[i][width] = '\0';
        display->lastBuffer[i][width] = '\0';
    }

    system("cls");
}

void cleanupDisplay(Display* display) {
    for (int i = 0; i < display->height; i++) {
        free(display->buffer[i]);
        free(display->lastBuffer[i]);
    }
    free(display->buffer);
    free(display->lastBuffer);
}

void clearDisplay(Display* display) {
    for (int y = 0; y < display->height; y++) {
        for (int x = 0; x < display->width; x++) {
            display->buffer[y][x] = EMPTY_CHAR;
        }
    }
}

void renderGame(Display* display, Game* game) {
    clearDisplay(display);
    drawBorder(display, 0, 0, display->width - 1, display->height - 1);

    renderPaddles(display, &game->leftPaddle, &game->rightPaddle);
    renderBall(display, &game->ball);
    renderScore(display, &game->score, game->gameWidth);
    renderUI(display, game);
}

void renderBall(Display* display, Ball* ball) {
    int x = getBallX(ball);
    int y = getBallY(ball);

    if (x >= 0 && x < display->width && y >= 0 && y < display->height) {
        setDisplayChar(display, x, y, BALL_CHAR);
    }
}

void renderPaddles(Display* display, Paddle* left, Paddle* right) {
    for (int i = 0; i < left->height; i++) {
        int y = left->y + i;
        if (y >= 0 && y < display->height) {
            setDisplayChar(display, left->x, y, PADDLE_CHAR);
        }
    }

    for (int i = 0; i < right->height; i++) {
        int y = right->y + i;
        if (y >= 0 && y < display->height) {
            setDisplayChar(display, right->x, y, PADDLE_CHAR);
        }
    }
}

void renderScore(Display* display, Score* score, int gameWidth) {
    char scoreText[50];
    sprintf(scoreText, "%d - %d", getLeftScore(score), getRightScore(score));

    int textLen = strlen(scoreText);
    int startX = (gameWidth - textLen) / 2;

    for (int i = 0; i < textLen && startX + i < display->width; i++) {
        setDisplayChar(display, startX + i, 1, scoreText[i]);
    }

    char rallyText[30];
    sprintf(rallyText, "Rally: %d", getCurrentRally(score));
    int rallyLen = strlen(rallyText);
    int rallyX = gameWidth - rallyLen - 2;

    for (int i = 0; i < rallyLen && rallyX + i < display->width; i++) {
        setDisplayChar(display, rallyX + i, display->height - 2, rallyText[i]);
    }
}

void renderUI(Display* display, Game* game) {
    char* controls = "W/S: Left Paddle | I/K: Right Paddle | P: Pause | Q: Quit | R: Reset";
    int controlsLen = strlen(controls);
    int startX = (display->width - controlsLen) / 2;

    if (startX < 0) startX = 0;

    for (int i = 0; i < controlsLen && startX + i < display->width; i++) {
        setDisplayChar(display, startX + i, display->height - 1, controls[i]);
    }
}

void renderMenu(Display* display, int selection) {
    clearDisplay(display);
    drawBorder(display, 0, 0, display->width - 1, display->height - 1);

    char* title = "PONG GAME";
    int titleLen = strlen(title);
    int titleX = (display->width - titleLen) / 2;

    for (int i = 0; i < titleLen; i++) {
        setDisplayChar(display, titleX + i, 3, title[i]);
    }

    char* options[] = {
        "1. Start Game",
        "2. Settings",
        "3. Statistics",
        "4. Exit"
    };

    int numOptions = 4;
    int startY = display->height / 2 - 2;

    for (int i = 0; i < numOptions; i++) {
        int optionLen = strlen(options[i]);
        int optionX = (display->width - optionLen) / 2;
        
        if (i == selection) {
            setDisplayChar(display, optionX - 2, startY + i, '>');
        }
        
        for (int j = 0; j < optionLen; j++) {
            setDisplayChar(display, optionX + j, startY + i, options[i][j]);
        }
    }
}

void renderGameOver(Display* display, int winner) {
    char* winText;
    if (winner == PLAYER_LEFT) {
        winText = "LEFT PLAYER WINS!";
    } else {
        winText = "RIGHT PLAYER WINS!";
    }

    int textLen = strlen(winText);
    int startX = (display->width - textLen) / 2;
    int startY = display->height / 2;

    for (int i = 0; i < textLen; i++) {
        setDisplayChar(display, startX + i, startY, winText[i]);
    }

    char* pressKey = "Press R to restart or Q to quit";
    int keyLen = strlen(pressKey);
    int keyX = (display->width - keyLen) / 2;

    for (int i = 0; i < keyLen; i++) {
        setDisplayChar(display, keyX + i, startY + 2, pressKey[i]);
    }
}

void renderPauseScreen(Display* display) {
    char* pauseText = "GAME PAUSED";
    int textLen = strlen(pauseText);
    int startX = (display->width - textLen) / 2;
    int startY = display->height / 2;
    
    for (int i = 0; i < textLen; i++) {
        setDisplayChar(display, startX + i, startY, pauseText[i]);
    }

    char* resumeText = "Press P to resume";
    int resumeLen = strlen(resumeText);
    int resumeX = (display->width - resumeLen) / 2;

    for (int i = 0; i < resumeLen; i++) {
        setDisplayChar(display, resumeX + i, startY + 2, resumeText[i]);
    }
}

void renderStats(Display* display, Score* score) {
    clearDisplay(display);
    drawBorder(display, 0, 0, display->width - 1, display->height - 1);

    char* title = "STATISTICS";
    int titleLen = strlen(title);
    int titleX = (display->width - titleLen) / 2;

    for (int i = 0; i < titleLen; i++) {
        setDisplayChar(display, titleX + i, 2, title[i]);
    }

    char stats[10][50];
    sprintf(stats[0], "Matches Played: %d", getMatchesPlayed(score));
    sprintf(stats[1], "Left Wins: %d", getLeftWins(score));
    sprintf(stats[2], "Right Wins: %d", getRightWins(score));
    sprintf(stats[3], "Total Bounces: %d", getTotalBounces(score));
    sprintf(stats[4], "Longest Rally: %d", getLongestRally(score));
    sprintf(stats[5], "Current Rally: %d", getCurrentRally(score));

    int startY = 5;
    for (int i = 0; i < 6; i++) {
        int statLen = strlen(stats[i]);
        int statX = (display->width - statLen) / 2;

        for (int j = 0; j < statLen; j++) {
            setDisplayChar(display, statX + j, startY + i, stats[i][j]);
        }
    }
}

void flushDisplay(Display* display) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);

    if (display->doubleBuffered) {
        for (int y = 0; y < display->height; y++) {
            for (int x = 0; x < display->width; x++) {
                if (display->buffer[y][x] != display->lastBuffer[y][x]) {
                    COORD pos = {x, y};
                    SetConsoleCursorPosition(hConsole, pos);
                    printf("%c", display->buffer[y][x]);
                    display->lastBuffer[y][x] = display->buffer[y][x];
                }
            }
        }
    } else {
        for (int y = 0; y < display->height; y++) {
            printf("%s\n", display->buffer[y]);
        }
    }
}

void setDisplayChar(Display* display, int x, int y, char c) {
    if (x >= 0 && x < display->width && y >= 0 && y < display->height) {
        display->buffer[y][x] = c;
    }
}

char getDisplayChar(Display* display, int x, int y) {
    if (x >= 0 && x < display->width && y >= 0 && y < display->height) {
        return display->buffer[y][x];
    }
    return EMPTY_CHAR;
}

void drawBorder(Display* display, int x1, int y1, int x2, int y2) {
    drawHorizontalLine(display, x1, y1, x2 - x1 + 1, WALL_CHAR);
    drawHorizontalLine(display, x1, y2, x2 - x1 + 1, WALL_CHAR);
    drawHorizontalLine(display, x1, y1, y2 - y1 + 1, WALL_CHAR);
    drawHorizontalLine(display, x2, y1, y2 - y1 + 1, WALL_CHAR);
}

void drawHorizontalLine(Display* display, int x, int y, int length, char c) {
    for (int i = 0; i < length; i++) {
        setDisplayChar(display, x + i, y, c);
    }
}

void drawVerticalLine(Display* display, int x, int y, int length, char c) {
    for (int i = 0; i < length; i++) {
        setDisplayChar(display, x, y + i, c);
    }
}

void setCursor(Display* display, int x, int y) {
    display->cursorX = x;
    display->cursorY = y;
}

void enableColor(Display* display) {
    display->colorEnabled = 1;
}

void disableColor(Display* display) {
    display->colorEnabled = 0;
}

void setDoubleBuffering(Display* display, int enabled) {
    display->doubleBuffered = enabled;
}

void swapBuffers(Display* display) {
    char** temp = display->buffer;
    display->buffer = display->lastBuffer;
    display->lastBuffer = temp;
}