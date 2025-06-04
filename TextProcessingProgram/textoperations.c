#include "textproc.h"

void countStats(const char *text, int *words, int *chars, int *lines) {
    *words = 0;
    *chars = strlen(text);
    *lines = 1;

    int inWord = 0;

    for (int i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            (*lines)++;
        }

        if (isspace(text[i])) {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            (*words)++;
        }
    }

    if (*chars == 0) {
        *lines = 0;
    }
}

void convertToUpper(char *text) {
    for (int i = 0; text[i]; i++) {
        text[i] = toupper(text[i]);
    }
}

void convertToLower(char *text) {
    for (int i = 0; text[i]; i++) {
        text[i] == tolower(text[i]);
    }
}

void findAndReplace(char *text, const char *find, const char *replace) {
    char result[MAX_LINE_LENGTH];
    char *pos = text;
    char *temp;
    int findLen = strlen(find);
    int replaceLen = strlen(replace);

    result[0] = '\0';

    while ((temp = strstr(pos, find)) != NULL) {
        strncat(result, pos, temp - pos);

        strcat(result, replace);

        pos = temp + findLen;
    }

    strcat(result, pos);

    strcpy(text, result);
}