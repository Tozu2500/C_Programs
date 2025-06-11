#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {

    char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
    char uppercase[] = "ABCDEFGHIJKLMONPQRTSUVWXYZ";
    char numbers[] = "0123456789";
    char symbols[] = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    char all_chars[100];

    int length;
    int include_upper;
    int include_numbers;
    int include_symbols;

    printf("Welcome! This is a password generator!\n");
    printf("Enter a password length you would like between 4 and 50: ");
    scanf("%d", &length);

    if (length < 4 || length > 50) {
        printf("Your length was not between 4 and 50, it will be auto assigned to 12 characters.\n");
        length = 12;
    }

    printf("Would you like to include uppercase letters? (1 = Yes, 0 = No): ");
    scanf("%d", &include_upper);

    printf("Would you like to include numbers? (1 = Yes, 0 = No): ");
    scanf("%d", &include_numbers);

    printf("Would you like to include symbols? (1 = Yes, 0 = No) ");
    scanf("%d", &include_symbols);

    // Build characters set
    strcpy(all_chars, lowercase);
    if (include_upper) strcat(all_chars, uppercase);
    if (include_numbers) strcat(all_chars, numbers);
    if (include_symbols) strcat(all_chars, symbols);

    int char_set_size = strlen(all_chars);

    // Seed random generator
    srand(time(NULL));

    printf("\n Generated password: ");
    for (int i = 0; i < length; i++) {
        int random_index = rand() % char_set_size;
        printf("%c", all_chars[random_index]);
    }
    printf("\n\n");

    return 0;
}