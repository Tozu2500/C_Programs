#ifndef INPUT_H
#define INPUT_H

#include "game.h"

typedef struct {
    int keyBuffer[256];
    int keyPressed[256];
    int keyReleased[256];
    int currentInput;
    int lastInput;
    int inputDelay;
    int repeatDelay;
    int initialized;
} Input;

void initInput(Input* input);
void updateInput(Input* input);
void processInput(Input* input, Game* game);
void handleMenuInput(Input* input, int* selection, int maxOptions);
int isKeyPressed(Input* input, int key);
int isKeyDown(Input* input, int key);
int isKeyReleased(Input* input, int key);
void clearInput(Input* input);
void setInputDelay(Input* input, int delay);
void setRepeatDelay(Input* input, int delay);
int getLastInput(Input* input);
void handleGameInput(Input* input, Game* game);
void handlePauseInput(Input* input, Game* game);
int kbhit(void);
int getch(void);
void setupConsole(void);
void restoreConsole(void);

#endif // INPUT_H