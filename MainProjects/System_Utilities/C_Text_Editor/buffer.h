#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct {
    char **lines;
    int line_count;
} TextBuffer;

// Lifecycle
void tbInit(TextBuffer *tb);
void tbFree(TextBuffer *tb);

// Query
const char* tbGetLine(const TextBuffer *tb, int row);
int tbLineLen(const TextBuffer *tb, int row);

// Editing primitives
void tbInsertChar(TextBuffer *tb, int row, int col, char c);
void tbInsertNewline(TextBuffer *tb, int row, int col);
void tbBackspaceAt(TextBuffer *tb, int row, int col);  // Deletes char before (row, col)

void tbDeleteAt(TextBuffer *tb, int row, int col);  // Deletes char at (row, col)

// Helpers
void tbClearAndSetLines(TextBuffer *tb, char **newLines, int count);

#endif  // BUFFER_H