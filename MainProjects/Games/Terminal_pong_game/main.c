#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "display.h"
#include "input.h"
#include "utils.h"
#include "types.h"

typedef enum {
    MENU_STATE,
    GAME_STATE,
    SETTINGS_STATE,
    STATS_STATE,
    GAME_OVER_STATE
} ProgramState;

typedef struct {
    ProgramState currentState;
    int menuSelection;
    int settingsSelection;
    int running;
    Game game;
    Display display;
    Input input;
    Timer timer;
} PongProgram;

void initProgram(PongProgram* program);
void cleanupProgram(PongProgram* program);
void updateProgram(PongProgram* program);
void renderProgram(PongProgram* program);
void handleMenuState(PongProgram* program);
void handleGameState(PongProgram* program);
void handleSettingsState(PongProgram* program);
void handleStatsState(PongProgram* program);
void handleGameOverState(PongProgram* program);
void changeState(PongProgram* program, ProgramState newState);
void showMainMenu(PongProgram* program);
void showSettings(PongProgram* program);
void showStatistics(PongProgram* program);
void startNewGame(PongProgram* program);

int main(void) {
    PongProgram program;

    seedRandom();
    initProgram(&program);

    while (program.running) {
        updateTimer(&program.timer);
        updateInput(&program.input);
        updateProgram(&program);
        renderProgram(&program);
        waitForFrame(&program.timer);
    }

    cleanupProgram(&program);
    return 0;
}

void initProgram(PongProgram* program) {
    program->currentState = MENU_STATE;
    program->menuSelection = 0;
    program->settingsSelection = 0;
    program->running = 1;

    initGame(&program->game, DEFAULT_GAME_WIDTH, DEFAULT_GAME_HEIGHT);
    initDisplay(&program->display, DEFAULT_GAME_WIDTH, DEFAULT_GAME_HEIGHT);
    initInput(&program->input);
    initTimer(&program->timer, 60);

    printf("Pong Game in C\n");
    printf("Loading...\n");
    sleepMs(1000);
}

void cleanUpProgram(PongProgram* program) {
    cleanupDisplay(&program->display);
    restoreConsole();
    printf("\nThanks for playing!\n");
}

void updateProgram(PongProgram* program) {
    switch (program->currentState) {
        case MENU_STATE:
            handleMenuState(program);
            break;
        case GAME_STATE:
            handleGameState(program);
            break;
        case SETTINGS_STATE:
            handleSettingsState(program);
            break;
        case STATS_STATE:
            handleStatsState(program);
            break;
        case GAME_OVER_STATE:
            handleGameOverState(program);
            break;
    }

    if (isKeyPressed(&program->input, 27)) {
        program->running = 0;
    }
}

void renderProgram(PongProgram* program) {
    switch (program->currentState) {
        case MENU_STATE:
            renderMenu(&program->display, program->menuSelection);
            break;
        case GAME_STATE:
            renderGame(&program->display, &program->name);
            if (isGamePaused(&program->game)) {
                renderPauseScreen(&program->display);
            }
            break;
        case SETTINGS_STATE:
            showSettings(program);
            break;
        case STATS_STATE:
            renderStats(&program->display, &program->game.score);
            break;
        case GAME_OVER_STATE:
            renderGameOver(&program->display, getGameWinner(&program->game));
            break;
    }

    flushDisplay(&program->display);
}

void handleMenuState(PongProgram* program) {
    handleMenuInput(&program->input, &program->menuSelection, 4);

    if (isKeyPressed(&program->input, 13) || isKeyPressed(&program->input, ' ')) {
        switch (program->menuSelection) {
            case 0:
                startNewGame(program);
                changeState(program, GAME_STATE);
                break;
            case 1:
                changeState(program, SETTINGS_STATE);
                break;
            case 2:
                changeState(program, STATS_STATE);
                break;
            case 3:
                program->running = 0;
                break;
        }
    }

    if (isKeyPressed(&program->input, '1')) {
        startNewGame(program);
        changeState(program, GAME_STATE);
    }

    if (isKeyPressed(&program->input, '2')) {
        changeState(program, SETTINGS_STATE);
    }

    if (isKeyPressed(&program->input, '3')) {
        changeState(program, STATS_STATE);
    }

    if (isKeyPressed(&program->input, '4') || isKeyPressed(&program->input, 'q') || isKeyPressed(&program->input, 'Q')) {
        program->running = 0;
    }
}

void handleGameState(PongProgram* program) {
    if (!isGamePaused(&program->game)) {
        updateGame(&program->game);
    }

    processInput(&program->input, &program->game);

    if (!isGameRunning(&program->game) && program->game.gameState == GAME_ENDED) {
        changeState(program, GAME_OVER_STATE);
    }

    if (isKeyPressed(&program->input, 27)) {
        changeState(program, MENU_STATE);
    }
}

void handleSettingsState(PongProgram* program) {
    char settingsText[20][50];
    sprintf(settingsText[0], "Game settings");
    sprintf(settingsText[1], "");
    sprintf(settingsText[2], "Game speed: %d", program->game.gameSpeed);
    sprintf(settingsText[3], "Difficulty: %s",
            program->game.difficulty == 1 ? "Easy" :
            program->game.difficulty == 2 ? "Medium" : "Hard");
    sprintf(settingsText[4], "Sound: %s", program->game.soundEnabled ? "On" : "Off");
    sprintf(settingsText[5], "AI Enabled: %s", isPaddleAI(&program->game.rightPaddle) ? "Yes" : "No");
    sprintf(settingsText[6], "");
    sprintf(settingsText[7], "Press ESC to return to the main menu");

    clearDisplay(&program->display);
}