#ifndef LIBRARY_H
#define LIBRARY_H

#include "book.h"

#define MAX_BOOKS 1000

typedef struct {
    Book* books[MAX_BOOKS];
    int book_count;
    int next_id;
} Library;

// Function prototypes
Library* library_create(void);
void library_destroy(Library* lib);
bool library_add_book(Library* lib, const char* title, const char* author, const char* isbn, int year);
Book* library_find_book_by_id(Library* lib, int id);
Book* library_find_book_by_title(Library* lib, const char* title);
void library_display_all_books(const Library* lib);
void library_display_available_books(const Library* lib);
bool library_checkout_book(Library* lib, int book_id);
bool library_return_book(Library* lib, int book_id);
void library_get_stats(const Library* lib, int* total, int* available, int* checked_out);

#endif // LIBRARY_H