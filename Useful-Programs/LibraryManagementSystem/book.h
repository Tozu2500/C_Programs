#ifndef BOOK_H
#define BOOK_H

#include <stdbool.h>

#define MAX_TITLE_LENGTH 100
#define MAX_AUTHOR_LENGTH 50
#define MAX_ISBN_LENGTH 20

typedef struct {
    int id;
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char isbn[MAX_ISBN_LENGTH];
    bool is_available;
    int year_published;
} Book;

// Function prototypes
Book* book_create(int id, const char* title, const char* author, const char* isbn, int year);
void book_destroy(Book* book);
void book_display(const Book* book);
bool book_is_available(const Book* book);
void book_checkout(Book* book);
void book_return(Book* book);

#endif // BOOK_H