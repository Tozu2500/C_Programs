How to use:

Compile with make or gcc -o xor_encrypt main.c xor_encrypt.c
Encrypt: ./xor_encrypt -e input.txt encrypted.bin mypassword
Decrypt: ./xor_encrypt -d encrypted.bin decrypted.txt mypassword

Important security note: XOR encryption with a repeating key is not cryptographically secure and should only be used for learning purposes or basic obfuscation. For real security needs, use established cryptographic libraries like OpenSSL.
The encryptor processes files in 4KB chunks to handle large files efficiently while keeping memory usage low.