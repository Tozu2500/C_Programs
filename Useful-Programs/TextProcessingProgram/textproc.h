#ifndef TEXTPROC_H
#define TEXTPROC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1000
#define MAX_WORD_LENGTH 100
#define MAX_FILENAME_LENGTH 256

// Function prototypes
// File operations
void processFile(const char* filename);

// Text analysis
void countStats(const char* text, int* words, int* chars, int* lines);

// Text manipulation
void convertToUpper(char* text);
void convertToLower(char *text);
void findAndReplace(char *text, const char *find, const char *replace);

// User interface (text)
void displayMenu();
void handleMenuChoice(int choice);

#endif