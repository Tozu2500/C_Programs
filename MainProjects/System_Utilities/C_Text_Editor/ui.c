#include "ui.h"
#include "buffer.h"
#include "editor.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

static HANDLE hOut;

void uiInit(struct EditorState *E) {
    (void)E;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    uiSetCursorVisible(true);
}

void uiSetCursorVisible(bool visible) {
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(hOut, &ci);
}

void uiGetWindowSize(int *rows, int *cols) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hOut, &info);
    *cols = info.srWindow.Right - info.srWindow.Left + 1;
    *rows = info.srWindow.Bottom - info.srWindow.Top + 1;
}

// Write a string at (x, y) padded to width 'cols'
static void put_line_at(int y, const char *s, int cols) {
    COORD pos = { 0, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written = 0;
    int len = (int)strlen(s);
    if (len > cols) len = cols;
    WriteConsoleA(hOut, s, (DWORD)len, &written, NULL);
    // Pad with spaces to clear the rest of the line
    for (int i = len; i < cols; ++i) {
        WriteConsoleA(hOut, " ", 1, &written, NULL);
    }
}

static void set_cursor(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
}

KeyEvent uiReadKey(void) {
    int c =_getch();
    if (c == 0 || c == 224) {
        int ext = _getch();
        switch (ext) {
            case 72: return (KeyEvent) { KEY_ARROW_UP, 0 };
            case 80: return (KeyEvent) { KEY_ARROW_DOWN, 0 };
            case 75: return (KeyEvent) { KEY_ARROW_LEFT, 0 };
            case 77: return (KeyEvent) { KEY_ARROW_RIGHT, 0 };
            case 71: return (KeyEvent) { KEY_HOME, 0 };
            case 79: return (KeyEvent) { KEY_END, 0 };
            case 73: return (KeyEvent) { KEY_PAGE_UP, 0 };
            case 81: return (KeyEvent) { KEY_PAGE_DOWN, 0 };
            case 83: return (KeyEvent) { KEY_DELETE, 0 };
            default: return (KeyEvent) { KEY_NONE, 0 };
        }
    } else {
        switch (c) {
            case 8: return (KeyEvent) { KEY_BACKSPACE, 0 };
            case 13: return (KeyEvent) { KEY_ENTER, 0 };
            case 27: return (KeyEvent) { KEY_ESC, 0 };
            case 14: return (KeyEvent) { KEY_CTRL_N, 0 };
            case 15: return (KeyEvent) { KEY_CTRL_O, 0 };
            case 17: return (KeyEvent) { KEY_CTRL_Q, 0 };
            case 19: return (KeyEvent) { KEY_CTRL_S, 0 };
            default:
                if (c >= 32 && c <= 126) {
                    return (KeyEvent) { KEY_CHAR, c };
                } else if (c == 9) {
                    return (KeyEvent) { KEY_CHAR, '\t' };
                } else {
                    return (KeyEvent) { KEY_NONE, 0 };
                }
        }
    }
}

// Draw editor content + status bars
void uiRefreshScreen(struct EditorState *E) {
    int rows = E->screenrows;
    int cols = E->screencols;

    // Draw text area (rows - 2)
    int textRows = rows - 2;
    for (int y = 0; y < textRows; ++y) {
        int filerow = E->rowoff + y;
        if (filerow >= E->buf.line_count) {
            put_line_at(y, "~", cols);
        } else {
            const char *line = tbGetLine(&E->buf, filerow);
            int len = tbLineLen(&E->buf, filerow);
            if (E->coloff < len) {
                line += E->coloff;
                len -= E->coloff;
            } else {
                line = "";
                len = 0;
            }
            char *tmp = (char*)_malloca((size_t)cols + 1);
            int n = len > cols ? cols : len;
            memcpy(tmp, line, (size_t)n);
            tmp[n] = '\0';
            put_line_at(y, tmp, cols);
            _freea(tmp);
        }
    }

    // Status bar
    char status[256], rstatus[128];
    const char *name = E->filename ? E->filename : "[No Name]";
    snprintf(status, sizeof status, " %-20.20s  %s | %d lines",
            name, E->dirty ? "(modified)" : "      ", E->buf.line_count);
    snprintf(rstatus, sizeof rstatus, " Ln %d, Col %d ",
            E->cy + 1, E->cx + 1);

    int leftLen = (int)strlen(status);
    int rightLen = (int)strlen(rstatus);
    char line[512];
    memset(line, 0, sizeof line);
    memcpy(line, status, (size_t)leftLen);

    // Right align rstatus
    int start = cols - rightLen;
    if (start < leftLen + 1) start = leftLen + 1;
    for (int i = leftLen; i < start; ++i) {
        line[i] = ' ';
    }
    if (start < cols) {
        memcpy(line + start, rstatus, (size_t)(cols - start));
    }
    line[cols] = '\0';
    put_line_at(rows - 2, line, cols);

    // Message bar
    char msg[256] = {0};
    if (E->statusmsg[0]) {
        snprintf(msg, sizeof msg, " %s", E->statusmsg);
    }
    put_line_at(rows - 1, msg, cols);

    // Cursor placement
    int cx = E->cx - E->coloff;
    int cy = E->cy - E->rowoff;
    if (cy >= 0 && cy < textRows && cx >= 0 && cx < cols) {
        set_cursor(cx, cy);
    } else {
        set_cursor(0, rows - 1);
    }
}