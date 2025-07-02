#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void clear_input_buffer(void);
int get_integer_input(const char* prompt);
void get_string_input(const char* prompt, char* buffer, int max_length);
void print_separator(void);
void print_header(const char* title);

#endif // UTILS_H