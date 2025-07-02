#include "book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Book* book_create(int id, const char* title, const char* author, const char* isbn, int year) {
    Book* book = malloc(sizeof(Book));
    if (!book) {
        return NULL;
    }

    book->id = id;
    strncpy(book->title, title, MAX_TITLE_LENGTH - 1);

    book->title[MAX_TITLE_LENGTH - 1] = '\0';
    strncpy(book->author, author, MAX_AUTHOR_LENGTH - 1);

    book->author[MAX_AUTHOR_LENGTH - 1] = '\0';
    strncpy(book->isbn, isbn, MAX_ISBN_LENGTH - 1);
    book->isbn[MAX_ISBN_LENGTH - 1] = '\0';
    book->is_available = true;
    book->year_published = year;

    return book;
}

void book_destroy(Book* book) {
    if (book) {
        free(book);
    }
}

void book_display(const Book* book) {
    if (!book) {
        printf("Invalid book!\n");
        return;
    }

    printf("ID: %d\n", book->id);
    printf("Title: %s\n", book->title);
    printf("Author: %s\n", book->author);
    printf("ISBN: %s\n", book->isbn);
    printf("Year: %d\n", book->year_published);
    printf("Status: %s\n", book->is_available ? "Available" : "Checked out");
}

bool book_is_available(const Book* book) {
    return book ? book->is_available : false;
}

void book_checkout(Book* book) {
    if (book) {
        book->is_available = false;
    }
}

void book_return(Book* book) {
    if (book) {
        book->is_available = true;
    }
}