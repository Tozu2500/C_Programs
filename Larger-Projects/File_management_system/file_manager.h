#ifndef FILEMANAGER_H
#define FILEMANAGER_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "data_structures.h"

// Function prototypes
int init_file_manager(FileManager **fm);
void cleanup_file_manager(FileManager *fm);

int add_file_record(FIleManager *fm, FileRecord *record);
int delete_file_record(FileManager *fm, const char *filename);
FileRecord *search_file_by_name(FileManager *fm, const char *filename);

void display_all_files(FileManager *fm);
void display_file_record(FileRecord *record);

void sort_files_by_size(FileManager *fm);
void sort_files_by_date(FileManager *fm);

void generate_statistics(FileManager *fm);
int export_to_csv(FileManager *fm, const char *filename);
int import_from_csv(FILEMANAGE *fm, const char *filename);

// Helper functions
FileRecord *create_file_record(const char *filename, long size);
void free_file_record(FileRecord *record);

#endif // FILEMANAGER_H