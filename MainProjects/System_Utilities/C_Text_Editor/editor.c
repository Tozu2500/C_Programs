#include "editor.h"
#include "fileio.h"
#include "ui.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUIT_TIMES 2

static void editorScroll(EditorState *E) {
    int screenTextRows = E->screenrows - 2;

    if (E->cy < E->rowoff) E->rowoff = E->cy;
    if (E->cy >= E->rowoff + screenTextRows) E->rowoff = E->cy - screenTextRows + 1;

    if (E->cx < E->coloff) E->coloff = E->cx;
    if (E->cx >= E->coloff + E->screencols) E->coloff = E->cx - E->screencols + 1;
}

void editorSetStatus(EditorState *E, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E->statusmsg, sizeof E->statusmsg, fmt, ap);
    va_end(ap);
    E->statusmsg_time = time(NULL);
}

static void editorUpdateWindowSize(EditorState *E) {
    uiGetWindowSize(&E->screenrows, &E->screencols);
    if (E->screenrows < 3) E->screenrows = 3;
}

void editorInit(EditorState *E) {
    memset(E, 0, sizeof *E);
    tbInit(&E->buf);
    E->cx = E->cy = 0;
    E->rowoff = E->coloff = 0;
    E->filename = NULL;
    E->dirty = 0;
    E->quit_times = QUIT_TIMES;
    E->statusmsg[0] = '\0';
    editorUpdateWindowSize(E);
    uiInit(E);
    editorSetStatus(E, "Ctrl+O Open | Ctrl+S Save | Ctrl+N New | Ctrl+Q Quit");
}

void editorFree(EditorState *E) {
    free(E->filename);
    tbFree(&E->buf);
}

void editorOpen(EditorState *E, const char *filename) {
    char err[256];
    if (!file_load(filename, &E->buf, err, sizeof err)) {
        editorSetStatus(E, "%s", err);
        return;
    }
    free(E->filename);
    E->filename = _strdup(filename);
    E->cx = E->cy = E->rowoff = E->coloff = 0;
    E->dirty = 0;
    editorSetStatus(E, "Opened: %s", filename);
}

int editorSave(EditorState *E) {
    char err[256];
    if (!E->filename) {
        char name[260] = {0};
        if (!editorPrompt(E, name, sizeof name, "Save as: %s (ESC to cancel)")) {
            editorSetStatus(E, "Save canceled");
            return 0;
        }
        free(E->filename);
        E->filename = _strdup(name);
    }
    if (file_save(E->filename, &E->buf, err, sizeof err)) {
        E->dirty = 0;
        editorSetStatus(E, "Saved to %s", E->filename);
        return 1;
    } else {
        editorSetStatus(E, "%s", err);
        return 0;
    }
}

static int clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void editorMoveCursor(EditorState *E, EditorKey key) {
    int row = E->cy;
    int col = E->cx;
    int lineLen = tbLineLen(&E->buf, row);

    switch (key) {
        case KEY_ARROW_LEFT:
            if (col > 0) col--;
            else if (row > 0) {
                row--;
                col = tbLineLen(&E->buf, row);
            }
            break;

        case KEY_ARROW_RIGHT:
            if (col < lineLen) col++;
            else if (row + 1 < E->buf.line_count) {
                row++;
                col = 0;
            }
            break;

        case KEY_ARROW_UP:
            if (row > 0) row--;
            break;

        case KEY_ARROW_DOWN:
            if (row + 1 < E->buf.line_count) row++;
            break;

        case KEY_HOME:
            col = 0;
            break;

        case KEY_END:
            col = lineLen;
            break;

        case KEY_PAGE_UP:
            for (int i = 0; i < E->screenrows - 2; ++i) {
                if (row > 0) row--;
            }
            break;

        case KEY_PAGE_DOWN:
            for (int i = 0; i < E->screenrows - 2; ++i) {
                if (row + 1 < E->buf.line_count) row++;
            }
            break;

        default:
            break;
    }

    lineLen = tbLineLen(&E->buf, row);
    col = clamp(col, 0, lineLen);

    E->cy = row;
    E->cx = col;
}

int editorPrompt(EditorState *E, char *out, size_t outsz, const char *fmt, ...) {
    out[0] = '\0';
    size_t len = 0;

    char prompt[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(prompt, sizeof prompt, fmt, ap);
    va_end(ap);

    while (1) {
        editorSetStatus(E, prompt, out);
        editorRefreshScreen(E);

        KeyEvent ev = uiReadKey();
        if (ev.type == KEY_ESC) {
            out[0] = '\0';
            editorSetStatus(E, "");
            return 0;
        } else if (ev.type == KEY_ENTER) {
            if (len > 0) {
                editorSetStatus(E, "");
                return 1;
            }
        } else if (ev.type == KEY_BACKSPACE) {
            if (len > 0) {
                out[--len] = '\0';
            }
        } else if (ev.type == KEY_CHAR) {
            int c = ev.ch;
            if (c == '\t') {
                for (int i = 0; i < 4 && len + 1 < outsz; ++i) out[len++] = ' ';
                out[len] = '\0';
            } else if (c >= 32 && c <= 126) {
                if (len + 1 < outsz) {
                    out[len++] = (char)c;
                    out[len] = '\0';
                }
            }
        }
    }
}

void editorRefreshScreen(EditorState *E) {
    editorUpdateWindowSize(E);
    editorScroll(E);
    uiRefreshScreen(E);
}

// Insert a character or handle tabs
static void editorInsertChar(EditorState *E, int ch) {
    if (ch == '\t') {
        int spaces = 4 - (E->cx % 4);
        for (int i = 0; i < spaces; ++i) {
            tbInsertChar(&E->buf, E->cy, E->cx, ' ');
            E->cx++;
        }
    } else {
        tbInsertChar(&E->buf, E->cy, E->cx, (char)ch);
        E->cx++;
    }
    E->dirty = 1;
}

static void editorInsertNewLine(EditorState *E) {
    tbInsertNewline(&E->buf, E->cy, E->cx);
    E->cy++;
    E->cx = 0;
    E->dirty = 1;
}

static void editorBackspace(EditorState *E) {
    if (E->cx == 0 && E->cy == 0) return;
    if (E->cx > 0) {
        tbBackspaceAt(&E->buf, E->cy, E->cx);
        E->cx--;
    } else {
        // Merge previous line
        int prevLen = tbLineLen(&E->buf, E->cy - 1);
        tbBackspaceAt(&E->buf, E->cy, 0);
        E->cy--;
        E->cx = prevLen;
    }
    E->dirty = 1;
}

static void editorDelete(EditorState *E) {
    tbDeleteAt(&E->buf, E->cy, E->cx);
    E->dirty = 1;
}

static void editorNew(EditorState *E) {
    tbFree(&E->buf);
    tbInit(&E->buf);
    free(E->filename);
    E->filename = NULL;
    E->cx = E->cy = E->rowoff = E->coloff = 0;
    editorSetStatus(E, "New file");
}

void editorProcessKeypress(EditorState *E) {
    KeyEvent ev = uiReadKey();

    // Auto clear transient message after ~5secs
    if (E->statusmsg[0] && (time(NULL) - E->statusmsg_time) > 5) {
        E->statusmsg[0] = '\0';
    }

    switch (ev.type) {
        case KEY_CHAR:
            editorInsertChar(E, ev.ch);
            break;
        case KEY_ENTER:
            editorInsertNewLine(E);
            break;
        case KEY_BACKSPACE:
            editorBackspace(E);
            break;
        case KEY_DELETE:
            editorDelete(E);
            break;
        
        case KEY_ARROW_LEFT: case KEY_ARROW_RIGHT:
        case KEY_ARROW_UP: case KEY_ARROW_DOWN:
        case KEY_HOME: case KEY_END:
        case KEY_PAGE_UP: case KEY_PAGE_DOWN:
            editorMoveCursor(E, ev.type);
            break;
        case KEY_CTRL_S:
            editorSave(E);
            break;
        case KEY_CTRL_O: {
            char name[260] = {0};
            if (editorPrompt(E, name, sizeof name, "Open file: %s (ESC to cancel)")) {
                editorOpen(E, name);
            } else {
                editorSetStatus(E, "Open canceled");
            }
        } break;
        case KEY_CTRL_N:
            if (E->dirty) {
                editorSetStatus(E, "Unsaved changes! Press CTRL+N again to discard.");
                E->quit_times = 1;
            } else {
                editorNew(E);
            }
            break;
        case KEY_CTRL_Q:
            if (E->dirty && E->quit_times > 0) {
                editorSetStatus(E, "Unsaved changes! Press CTRL+Q %d more time%s to quit.",
                E->quit_times, E->quit_times == 1 ? "" : "S");
                E->quit_times--;
                return;
            }
            uiSetCursorVisible(true);
            system("cls");
            exit(0);
        case KEY_ESC:
        case KEY_NONE:
            break;
    }
}