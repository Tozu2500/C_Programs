#include "input.h"
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

static HANDLE hStdin;
static DWORD fdwSaveOldMode;

void initInput(Input* input) {
    for (int i = 0; i < 256; i++) {
        input->keyBuffer[i] = 0;
        input->keyPressed[i] = 0;
        input->keyReleased[i] = 0;
    }

    input->currentInput = 0;
    input->lastInput = 0;
    input->inputDelay = 0;
    input->repeatDelay = 100;
    input->initialized = 1;

    setupConsole();
}

void updateInput(Input* input) {
    for (int i = 0; i < 256; i++) {
        input->keyPressed[i] = 0;
        input->keyReleased[i] = 0;
    }

    if (kbhit()) {
        int key = _getch();

        if (!input->keyBuffer[key]) {
            input->keyPressed[key] = 1;
        }

        input->keyBuffer[key] = 1;
        input->currentInput = key;
    } else {
        for (int i = 0; i < 256; i++) {
            if (input->keyBuffer[i]) {
                input->keyReleased[i] = 1;
                input->keyBuffer[i] = 0;
            }
        }
        input->currentInput = 0;
    }

    if (input->inputDelay > 0) {
        input->inputDelay--;
    }
}

void processInput(Input* input, Game* game) {
    if (isGamePaused(game)) {
        handlePauseInput(input, game);
    } else {
        handleGameInput(input, game);
    }
}

void handleMenuInput(Input* input, int* selection, int maxOptions) {
    if (isKeyPressed(input, 'w') || isKeyPressed(input, 'W')) {
        *selection = (*selection - 1 + maxOptions) % maxOptions;
    }

    if (isKeyPressed(input, 's') || isKeyPressed(input, 'S')) {
        *selection = (*selection + 1) % maxOptions;
    }
}

int isKeyPressed(Input* input, int key) {
    return input->keyPressed[key];
}

int isKeyDown(Input* input, int key) {
    return input->keyBuffer[key];
}

int isKeyReleased(Input* input, int key) {
    return input->keyReleased[key];
}

void clearInput(Input* input) {
    for (int i = 0; i < 256; i++) {
        input->keyBuffer[i] = 0;
        input->keyPressed[i] = 0;
        input->keyReleased[i] = 0;
    }
    input->currentInput = 0;
    input->lastInput = 0;
}

void setInputDelay(Input* input, int delay) {
    input->inputDelay = delay;
}

void setRepeatDelay(Input* input, int delay) {
    input->repeatDelay = clamp(delay, 1, 1000);
}

int getLastInput(Input* input) {
    return input->lastInput;
}

void handleGameInput(Input* input, Game* game) {
    if (isKeyPressed(input, QUIT_KEY) || isKeyPressed(input, QUIT_KEY - 32)) {
        game->running = 0;
    }

    if (isKeyPressed(input, PAUSE_KEY) || isKeyPressed(input, PAUSE_KEY - 32)) {
        pauseGame(game);
    }

    if (isKeyPressed(input, RESET_KEY) || isKeyPressed(input, RESET_KEY - 32)) {
        resetGame(game);
    }

    if (isKeyDown(input, UP_KEY) || isKeyDown(input, UP_KEY - 32)) {
        movePaddleUp(&game->leftPaddle);
    }

    if (isKeyDown(input, DOWN_KEY) || isKeyDown(input, DOWN_KEY - 32)) {
        movePaddleDown(&game->leftPaddle);
    }

    if (!isPaddleAI(&game->rightPaddle)) {
        if (isKeyDown(input, UP_KEY_2) || isKeyDown(input, UP_KEY_2 - 32)) {
            movePaddleUp(&game->rightPaddle);
        }

        if (isKeyDown(input, DOWN_KEY_2) || isKeyDown(input, DOWN_KEY_2 - 32)) {
            movePaddleDown(&game->rightPaddle);
        }
    }

    if (isKeyPressed(input, '1')) {
        setDifficulty(game, EASY_DIFFICULTY);
    }

    if (isKeyPressed(input, '2')) {
        setDifficulty(game, MEDIUM_DIFFICULTY);
    }

    if (isKeyPressed(input, '3')) {
        setDifficulty(game, HARD_DIFFICULTY);
    }

    if (isKeyPressed(input, '+') || isKeyPressed(input, '=')) {
        setGameSpeed(game, game->gameSpeed + 5);
    }

    if (isKeyPressed(input, '-') || isKeyPressed(input, '_')) {
        setGameSpeed(game, game->gameSpeed - 5);
    }

    if (isKeyPressed(input, 'm') || isKeyPressed(input, 'M')) {
        toggleSound(game);
    }

    if (isKeyPressed(input, 'a') || isKeyPressed(input, 'A')) {
        if (isPaddleAI(&game->rightPaddle)) {
            disableAI(&game->rightPaddle);
        } else {
            enableAI(&game->rightPaddle, game->difficulty);
        }
    }
}

void handlePauseInput(Input* input, Game* game) {
    if (isKeyPressed(input, PAUSE_KEY) || isKeyPressed(input, PAUSE_KEY - 32)) {
        resumeGame(game);
    }

    if (isKeyPressed(input, QUIT_KEY) || isKeyPressed(input, QUIT_KEY - 32)) {
        game->running = 0;
    }

    if (isKeyPressed(input, RESET_KEY) || isKeyPressed(input, RESET_KEY - 32)) {
        resetGame(game);
        resumeGame(game);
    }
}

int kbhit(void) {
    return _kbhit();
}

int getch(void) {
    return _getch();
}

void setupConsole(void) {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (hStdin == INVALID_HANDLE_VALUE) {
        return;
    }

    if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
        return;
    }

    DWORD fdwMode = fdwSaveOldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

    if (!SetConsoleMode(hStdin, fdwMode)) {
        return;
    }

    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void restoreConsole(void) {
    if (hStdin != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hStdin, fdwSaveOldMode);
    }

    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}