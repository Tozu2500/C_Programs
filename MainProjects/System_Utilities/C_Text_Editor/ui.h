#ifndef UI_H
#define UI_H

#include <stdbool.h>

struct EditorState;  // Forward

typedef enum {
    KEY_NONE = 0,
    KEY_ARROW_LEFT,
    KEY_ARROW_RIGHT,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_HOME,
    KEY_END,
    KEY_DELETE,
    KEY_BACKSPACE,
    KEY_ENTER,
    KEY_CTRL_S,
    KEY_CTRL_O,
    KEY_CTRL_N,
    KEY_CTRL_Q,
    KEY_ESC,
    KEY_CHAR
} EditorKey;

typedef struct {
    EditorKey type;
    int ch;  // Valid when type == KEY_CHAR
} KeyEvent;

// UI Lifecycle and rendering
void uiInit(struct EditorState *E);
void uiGetWindowSize(int *rows, int *cols);
void uiRefreshScreen(struct EditorState *E);

// Input
KeyEvent uiReadKey(void);

// Cursor show/hide
void uiSetCursorVisible(bool visible);

#endif  // UI_H