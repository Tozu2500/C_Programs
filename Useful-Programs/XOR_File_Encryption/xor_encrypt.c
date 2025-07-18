#include "xor_encrypt.h"

void xor_buffer(unsigned char* buffer, size_t buffer_size, const char* key, size_t key_len) {
    for (size_t i = 0; i < buffer_size; i++) {
        buffer[i] ^= key[i % key_len];
    }
}

int encrypt_file(const char* input_file, const char* output_file, const char* key) {
    FILE* input = fopen(input_file, "rb");
    if (!input) {
        printf("Error, can't open input file '%s'\n", input_file);
        return -1;
    }

    FILE* output = fopen(output_file, "wb");
    if (!output) {
        printf("Error, can't create output file '%s'\n", output_file);
        fclose(input);
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t key_len = strlen(key);

    if (key_len == 0) {
        printf("Error, the key can't be empty!\n");
        fclose(input);
        fclose(output);
        return -1;
    }

    printf("Encrypting '%s' to '%s' ...\n", input_file, output_file);

    while((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
        xor_buffer(buffer, bytes_read, key, key_len);

        if (fwrite(buffer, 1, bytes_read, output) != bytes_read) {
            printf("Error, failed to write output file!\n");
            fclose(input);
            fclose(output);
            return -1;
        }
    }

    fclose(input);
    fclose(output);
    printf("Encryption completed!\n");
    return 0;
}

int decrypt_file(const char* input_file, const char* output_file, const char* key) {
    printf("Decrypting '%s' to '%s' ...\n", input_file, output_file);
    int result = encrypt_file(input_file, output_file, key);
    if (result == 0) {
        printf("Decryption completed!\n");
    }
    return result;
}

void print_usage(const char* program_name) {
    printf("Usage: %s [options] <input_file> <output_file> <key>\n", program_name);
    printf("--- Options: ---\n");
    printf("    -e, --encrypt   Encrypt the input file (default)\n");
    printf("    -d, --decrypt   Decrypt the input file\n");
    printf("    -h, --help      Show this help message\n");
    printf("\nExample:\n");
    printf("  %s -e document.txt encrypted.bin mypassword\n", program_name);
    printf("  %s -d encrypted.bin decrypted.txt mypassword\n", program_name);
}