#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"
#include "ui.h"
#include <time.h>
#include <stdarg.h>

typedef struct EditorState {
    TextBuffer buf;
    int cx, cy;
    int rowoff, coloff;
    int screenrows, screencols;
    char *filename;
    int dirty;
    char statusmsg[128];
    time_t statusmsg_time;
    int quit_times;
} EditorState;

// Lifecycle
void editorInit(EditorState *E);
void editorFree(EditorState *E);

// Actions
void editorOpen(EditorState *E, const char *filename);
int editorSave(EditorState *E);

// UI Helpers
void editorSetStatus(EditorState *E, const char *fmt, ...);
int editorPrompt(EditorState *E, char *buf, size_t bufsz, const char *fmt, ...);

// Loop parts
void editorRefreshScreen(EditorState *E);
void editorProcessKeypress(EditorState *E);

#endif  // EDITOR_H