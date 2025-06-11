#include "textproc.h"

void processFile(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Error, the file couldn't be opened '%s'\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int totalWords = 0;
    int totalChars = 0;
    int totalLines = 0;

    printf("\nFile contents:\n");
    printf("_________________________\n");

    while(fgets(line, sizeof(line), file)) {
        printf("%s", line);

        int words, chars, lines;
        countStats(line, &words, &chars, &lines);
        totalWords += words;
        totalChars += chars;
        totalLines += lines;
    }

    printf("_________________________\n");
    printf("File statistics: \n");
    printf("  Total lines: %d\n", totalLines);
    printf("  Total words: %d\n", totalWords);
    printf("  Total characters: %d\n", totalChars);

    fclose(file);
}