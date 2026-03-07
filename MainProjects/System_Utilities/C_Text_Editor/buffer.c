#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static char* str_dup_range(const char *s, size_t n) {
    char *p = (char*)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static char* str_dup(const char *s) {
    return str_dup_range(s, strlen(s));
}

static void insert_line_at(TextBuffer *tb, int at, char *line) {
    if (at < 0) at = 0;
    if (at > tb->line_count) at = tb->line_count;
    tb->lines = (char**)realloc(tb->lines, sizeof(char*) * (tb->line_count + 1));
    memmove(&tb->lines[at + 1], &tb->lines[at], sizeof(char*) * (tb->line_count - at));
    tb->lines[at] = line ? line : str_dup("");
    tb->line_count++;
}

static void delete_line_at(TextBuffer *tb, int at) {
    if (at < 0 || at >= tb->line_count) return;
    free(tb->lines[at]);
    memmove(&tb->lines[at], &tb->lines[at + 1], sizeof(char*) * (tb->line_count - at - 1));
    tb->line_count--;
    
    if (tb->line_count == 0) {
        free(tb->lines);
        tb->lines = NULL;
    } else {
        tb->lines = (char**)realloc(tb->lines, sizeof(char*) * tb->line_count);
    }
}

void tbInit(TextBuffer *tb) {
    tb->lines = NULL;
    tb->line_count = 0;
    insert_line_at(tb, 0, str_dup(""));
}

void tbFree(TextBuffer *tb) {
    if (!tb) return;
    for (int i = 0; i < tb->line_count; ++i) free(tb->lines[i]);
    free(tb->lines);
    tb->lines = NULL;
    tb->line_count = 0;
}

const char* tbGetLine(const TextBuffer *tb, int row) {
    if (row < 0 || row >= tb->line_count) return "";
    return tb->lines[row];
}

int tbLineLen(const TextBuffer *tb, int row) {
    if (row < 0 || row >= tb->line_count) return 0;
    return (int)strlen(tb->lines[row]);
}

void tbInsertChar(TextBuffer *tb, int row, int col, char c) {
    if (row < 0) row = 0;
    if (row >= tb->line_count) row = tb->line_count - 1;

    char *line = tb->lines[row];
    int len = (int)strlen(line);
    if (col < 0) col = 0;
    if (col > len) col = len;

    char *newLine = (char*)malloc(len + 2);
    memcpy(newLine, line, col);
    newLine[col] = c;
    memcpy(newLine + col + 1, line + col, len - col + 1);
    free(tb->lines[row]);
    tb->lines[row] = newLine;
}

void tbInsertNewline(TextBuffer *tb, int row, int col) {
    if (row < 0) row = 0;
    if (row >= tb->line_count) row = tb->line_count - 1;

    char *line = tb->lines[row];
    int len = (int)strlen(line);
    if (col < 0) col = 0;
    if (col > len) col = len;

    char *left = str_dup_range(line, (size_t)col);
    char *right = str_dup(line + col);
    free(tb->lines[row]);
    tb->lines[row] = left;
    insert_line_at(tb, row + 1, right);
}

void tbBackspaceAt(TextBuffer *tb, int row, int col) {
    // Deletes char before (row, col)
    if (row < 0 || row >= tb->line_count) return;
    if (col > 0) {
        char *line = tb->lines[row];
        int len = (int)strlen(line);
        if (col > len) col = len;
        memmove(line + col - 1, line + col, (size_t)(len - col + 1));
    } else if (row > 0) {
        // Merge with previous line
        int prev = row - 1;
        int prevLen = (int)strlen(tb->lines[prev]);
        int curLen = (int)strlen(tb->lines[row]);
        tb->lines[prev] = (char*)realloc(tb->lines[prev], (size_t)(prevLen + curLen + 1));
        memcpy(tb->lines[prev] + prevLen, tb->lines[row], (size_t)(curLen + 1));
        delete_line_at(tb, row);
    }
}

void tbDeleteAt(TextBuffer *tb, int row, int col) {
    if (row < 0 || row >= tb->line_count) return;
    char *line = tb->lines[row];
    int len = (int)strlen(line);
    if (col < len) {
        memmove(line + col, line + col + 1, (size_t)(len - col));
    } else if (row + 1 < tb->line_count) {
        // Join with next line
        int nextLen = (int)strlen(tb->lines[row + 1]);
        tb->lines[row] = (char*)realloc(tb->lines[row], (size_t)(len + nextLen + 1));
        memcpy(tb->lines[row] + len, tb->lines[row + 1], (size_t)(nextLen + 1));
        delete_line_at(tb, row + 1);
    }
}

void tbClearAndSetLines(TextBuffer *tb, char **newLines, int count) {
    tbFree(tb);
    tb->lines = newLines;
    tb->line_count = count;
    if (tb->line_count == 0) {
        insert_line_at(tb, 0, str_dup(""));
    }
}