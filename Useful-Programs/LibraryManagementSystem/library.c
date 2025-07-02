#include "library.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Library* library_create(void) {
    Library* lib = malloc(sizeof(Library));
    if (!lib) {
        return NULL;
    }

    lib->book_count = 0;
    lib->next_id = 1;

    // Initialize all book pointer to NULL
    for (int i = 0; i < MAX_BOOKS; i++) {
        lib->books[i] = NULL;
    }

    return lib;
}

void library_destroy(Library* lib) {
    if (!lib) {
        return;
    }

    // Free all books
    for (int i = 0; i < lib->book_count; i++) {
        book_destroy(lib->books[i]);
    }

    free(lib);
}

bool library_add_book(Library* lib, const char* title, const char* author, const char* isbn, int year) {
    if (!lib || lib->book_count >= MAX_BOOKS) {
        return false;
    }

    Book* new_book = book_create(lib->next_id, title, author, isbn, year);
    if (!new_book) {
        return false;
    }

    lib->books[lib->book_count] = new_book;
    lib->book_count++;
    lib->next_id++;

    return true;
}

Book* library_find_book_by_id(Library* lib, int id) {
    if (!lib) {
        return NULL;
    }

    for (int i = 0; i < lib->book_count; i++) {
        if (lib->books[i] && lib->books[i]->id == id) {
            return lib->books[i];
        }
    }

    return NULL;
}

Book* library_find_book_by_title(Library* lib, const char* title) {
    if (!lib || !title) {
        return NULL;
    }

    for (int i = 0; i < lib->book_count; i++) {
        if (lib->books[i] && strcmp(lib->books[i]->title, title) == 0) {
            return lib->books[i];
        }
    }

    return NULL;
}

void library_display_all_books(const Library* lib) {
    if (!lib || lib->book_count == 0) {
        printf("No books in the library.\n");
        return;
    }

    print_header("All books in the Library:");
    for (int i = 0; i < lib->book_count; i++) {
        if (lib->books[i]) {
            book_display(lib->books[i]);
            print_separator();
        }
    }
}

void library_display_available_books(const Library* lib) {
    if (!lib) {
        printf("Invalid library.\n");
        return;
    }

    bool found_available = false;
    print_header("Available books:");

    for (int i = 0; i < lib->book_count; i++) {
        if (lib->books[i] && book_is_available(lib->books[i])) {
            book_display(lib->books[i]);
            print_separator();
            found_available = true;
        }
    }

    if (!found_available) {
        printf("No books are currently available.\n");
    }
}

bool library_checkout_book(Library* lib, int book_id) {
    Book* book = library_find_book_by_id(lib, book_id);
    if (!book) {
        printf("Book with ID %d was not found\n", book_id);
        return false;
    }

    if (!book_is_available(book)) {
        printf("Book '%s' is already checked out.\n", book->title);
        return false;
    }

    book_checkout(book);
    printf("Successfully checked out '%s'.\n", book->title);
    return true;
}

bool library_return_book(Library* lib, int book_id) {
    Book* book = library_find_book_by_id(lib, book_id);
    if (!book) {
        printf("Book with an ID of %d was not found.\n", book_id);
        return false;
    }

    if (book_is_available(book)) {
        printf("Book '%s' is already available.\n", book->title);
        return false;
    }

    book_return(book);
    printf("Successfully returned '%s'.\n", book->title);
    return true;
}

void library_get_stats(const Library* lib, int* total, int* available, int* checked_out) {
    if (!lib || !total || !available || !checked_out) {
        return;
    }

    *total = lib->book_count;
    *available = 0;
    *checked_out = 0;

    for (int i = 0; i < lib->book_count; i++) {
        if (lib->books[i]) {
            if (book_is_available(lib->books[i])) {
                (*available)++;
            } else {
                (*checked_out)++;
            }
        }
    }
}