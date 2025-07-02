#include "library.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void display_menu(void) {
    print_header("Library Management System");
    printf("1. Add a book\n");
    printf("2. Display all books\n");
    printf("3. Display available books\n");
    printf("4. Search book by ID\n");
    printf("5. Checkout a book\n");
    printf("6. Return a book\n");
    printf("7. Library statistics\n");
    printf("8. Exit\n");
    print_separator();
}

void add_book_menu(Library* lib) {
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char isbn[MAX_ISBN_LENGTH];
    int year;
    
    print_header("Add New Book");
    
    get_string_input("Enter book title: ", title, MAX_TITLE_LENGTH);
    get_string_input("Enter author name: ", author, MAX_AUTHOR_LENGTH);
    get_string_input("Enter ISBN: ", isbn, MAX_ISBN_LENGTH);
    year = get_integer_input("Enter publication year: ");
    
    if (library_add_book(lib, title, author, isbn, year)) {
        printf("\nBook added successfully!\n");
    } else {
        printf("\nFailed to add book. Library might be full.\n");
    }
}

void search_book_menu(Library* lib) {
    int book_id = get_integer_input("Enter book ID to search: ");
    Book* book = library_find_book_by_id(lib, book_id);
    
    if (book) {
        print_header("Book Found");
        book_display(book);
    } else {
        printf("Book with ID %d not found.\n", book_id);
    }
}

void checkout_book_menu(Library* lib) {
    int book_id = get_integer_input("Enter book ID to checkout: ");
    library_checkout_book(lib, book_id);
}

void return_book_menu(Library* lib) {
    int book_id = get_integer_input("Enter book ID to return: ");
    library_return_book(lib, book_id);
}

void display_stats(Library* lib) {
    int total, available, checked_out;
    library_get_stats(lib, &total, &available, &checked_out);
    
    print_header("Library Statistics");
    printf("Total books: %d\n", total);
    printf("Available books: %d\n", available);
    printf("Checked out books: %d\n", checked_out);
}

void initialize_sample_data(Library* lib) {
    library_add_book(lib, "The C Programming Language", "Brian Kernighan", "978-0131103627", 1988);
    library_add_book(lib, "Clean Code", "Robert Martin", "978-0132350884", 2008);
    library_add_book(lib, "Design Patterns", "Gang of Four", "978-0201633612", 1994);
    library_add_book(lib, "Algorithms", "Robert Sedgewick", "978-0321573513", 2011);
    library_add_book(lib, "Computer Systems", "Randal Bryant", "978-0134092669", 2015);
}

int main(void) {
    Library* lib = library_create();
    if (!lib) {
        printf("Failed to create library!\n");
        return 1;
    }
    
    // Add some sample books
    initialize_sample_data(lib);
    
    int choice;
    do {
        display_menu();
        choice = get_integer_input("Enter your choice (1-8): ");
        
        switch (choice) {
            case 1:
                add_book_menu(lib);
                break;
            case 2:
                library_display_all_books(lib);
                break;
            case 3:
                library_display_available_books(lib);
                break;
            case 4:
                search_book_menu(lib);
                break;
            case 5:
                checkout_book_menu(lib);
                break;
            case 6:
                return_book_menu(lib);
                break;
            case 7:
                display_stats(lib);
                break;
            case 8:
                printf("Thank you for using the Library Management System!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        
        if (choice != 8) {
            printf("\nPress Enter to continue...");
            getchar();
        }
        
    } while (choice != 8);
    
    library_destroy(lib);
    return 0;
}