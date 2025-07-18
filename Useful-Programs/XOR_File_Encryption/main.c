#include "xor_encrypt.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    int encrypt_mode = 1;
    int arg_offset = 1;

    if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encrypt") == 0) {
        encrypt_mode = 1;
        arg_offset = 2;
    } else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decrypt") == 0) {
        encrypt_mode = 0;
        arg_offset = 2;
    } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (argc < arg_offset + 3) {
        printf("Error, missing required arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    const char* input_file = argv[arg_offset];
    const char* output_file = argv[arg_offset + 1];
    const char* key = argv[arg_offset + 2];

    if (strlen(key) > MAX_KEY_LENGTH) {
        printf("Error, the key is too long (maximum %d characters allowed)\n", MAX_KEY_LENGTH);
        return 1;
    }

    int result;
    if (encrypt_mode) {
        result = encrypt_file(input_file, output_file, key);
    } else {
        result = decrypt_file(input_file, output_file, key);
    }
    
    return result;
}