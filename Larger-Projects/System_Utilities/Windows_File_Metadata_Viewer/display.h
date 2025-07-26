#ifndef DISPLAY_H
#define DISPLAY_H

#include "file_utils.h"

void display_banner(void);
void display_file_metadata(const FileInfo* info);
void display_attributes_table(const FileInfo* info);
void display_size_analysis(const FileInfo* info);
void display_time_information(const FileInfo* info);
void display_error(const char* message);
void display_help(void);
void clear_screen(void);
void pause_for_input(void);

#endif // DISPLAY_H