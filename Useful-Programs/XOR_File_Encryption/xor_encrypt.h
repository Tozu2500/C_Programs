#ifndef XOR_ENCRYPT_H
#define XOR_ENCRYPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int encrypt_file(const char* input_file, const char* output_file, const char* key);
int decrypt_file(const char* input_file, const char* output_file, const char* key);
void xor_buffer(unsigned char* buffer, size_t buffer_size, const char* key, size_t key_len);
void print_usage(const char* program_name);

#define BUFFER_SIZE 4096
#define MAX_KEY_LENGTH 256

#endif // XOR_ENCRYPT_H