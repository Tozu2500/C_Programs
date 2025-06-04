#include "textproc.h"

void displayMenu() {
    printf("Choose an option\n");
    printf("1. Process file (analyze an existing text file)\n");
    printf("2. Count words or characters in text\n");
    printf("3. Convert text to uppercase\n");
    printf("4. Convert text to lowercase\n");
    printf("5. Find and replace some words\n");
    printf("6. Exit the program\n");
}

void handleMenuChoice(int choice) {
    char filename[MAX_FILENAME_LENGTH];
    char text[MAX_LINE_LENGTH];
    char find[MAX_WORD_LENGTH], replace[MAX_WORD_LENGTH];

    switch(choice) {
        case 1:
            printf("Enter a filepath: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0; // Remove newline
            processFile(filename);
            break;
        case 2:
            printf("Enter text to analyze: ");
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;

            int words, chars, lines;
            countStats(text, &words, &chars, &lines);
            printf("Statistics\n");
            printf("  Words: %d\n", words);
            printf("  Characters: %d\n", chars);
            printf("  Lines: %d\n", lines);
            break;
        case 3:
            printf("Enter text to make uppercase: \n");
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            convertToUpper(text);
            printf("Uppercase: %s\n", text);
            break;
        case 4:
            printf("Enter text to make lowercase: \n");
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            convertToLower(text);
            printf("Lowercase %s\n", text);
            break;
        case 5:
            printf("Enter text: ");
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;

            printf("Enter a word to find: ");
            fgets(find, sizeof(find), stdin);
            find[strcspn(find, "\n")] = 0;

            printf("Enter a replacement word: ");
            fgets(replace, sizeof(replace), stdin);
            replace[strcspn(replace, "\n")] = 0;

            findAndReplace(text, find, replace);
            printf("Result: %s\n", text);
            break;

        default:
            printf("Invalid choice. Please try again\n");
    }
}