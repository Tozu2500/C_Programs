#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void normalize_newlines(char *buf, size_t *len) {
    // Convert CRLF -> LF in buffer in place
    size_t w = 0;
    for (size_t r = 0; r < *len; ++r) {
        if (buf[r] == '\r') {
            if (r + 1 < *len && buf[r + 1] == '\n') {
                continue;  // Skip \r in CRLF
            } else {
                buf[w++] = '\n';
            }
        } else {
            buf[w++] = buf[r];
        }
    }
    *len = w;
    buf[w] = '\0';
}

bool file_load(const char *filename, TextBuffer *tb, char *err, size_t errsz) {
    err[0] = '\0';
    FILE *f = fopen(filename, "rb");
    if (!f) {
        snprintf(err, errsz, "Cannot open: %s", filename);
        return false;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) sz = 0;

    char *buf = (char*)malloc((size_t)sz + 2);
    if (!buf) {
        fclose(f);
        snprintf(err, errsz, "Out of memory");
        return false;
    }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[n] = '\0';
    size_t len = n;
    normalize_newlines(buf, &len);

    // Split by '\n' into lines
    int cap = 16;
    int count = 0;
    char **lines = (char**)malloc(sizeof(char*) * cap);
    char *start = buf;
    for (size_t i = 0; i <= len; ++i) {
        if (i == len || buf[i] == '\n') {
            size_t seglen = (size_t)(&buf[i] - start);
            if (count == cap) {
                cap *= 2;
                lines = (char**)realloc(lines, sizeof(char*) * cap);
            }
            char *line = (char*)malloc(seglen + 1);
            memcpy(line, start, seglen);
            line[seglen] = '\0';
            lines[count++] = line;
            start = &buf[i + 1];
        }
    }
    free(buf);

    tbClearAndSetLines(tb, lines, count);
    return true;
}

bool file_save(const char *filename, const TextBuffer *tb, char *err, size_t errsz) {
    err[0] = '\0';
    FILE *f = fopen(filename, "wb");
    if (!f) {
        snprintf(err, errsz, "Cannot write: %s", filename);
        return false;
    }

    for (int i = 0; i < tb->line_count; ++i) {
        const char *line = tb->lines[i];
        size_t len = strlen(line);
        if (len && fwrite(line, 1, len, f) != len) {
            snprintf(err, errsz, "Write error");
            fclose(f);
            return false;
        }
        // Windows CRLF
        if (fwrite("\r\n", 1, 2, f) != 2) {
            snprintf(err, errsz, "Write error");
            fclose(f);
            return false;
        }
    }

    fclose(f);
    return true;
}