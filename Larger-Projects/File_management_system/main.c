#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "file_manager.h"
#include "data_structures.h"
#include "utils.h"
#include "logger.h"

void print_menu() {
    printf("\n --- Advanced File Manager System ---\n");
    printf("1. Create file record\n");
    printf("2. Display all files\n");
    printf("3. Search file by name\n");
    printf("4. Delete file record\n");
    printf("5. Sort files by their size\n");
    printf("6. Sort files by their date\n");
    printf("7. Generate the Statistics file\n");
    printf("8. Export to a CSV file\n");
    printf("9. Import from a CSV file\n");
    printf("0. Exit\n");
    printf("Choice: ");
}

int main() {
    FileManager *fm = NULL;
    Logger *logger = NULL;

    int choice;
    char filename[MAX_FILENAME];
    long size;

    // Initialize system
    if (init_file_manager(&fm) != SUCCESS) {
        fprintf(stderr, "Failed to initialize file manager.\n");
        return EXIT_FAILURE;
    }

    if (init_logger(&logger, "system.lgo"), != SUCCESS) {
        fprintf(stderr, "Failed to initialize logger.\n");
        cleanup_file_manager(fm);
        return EXIT_FAILURE;
    }
}