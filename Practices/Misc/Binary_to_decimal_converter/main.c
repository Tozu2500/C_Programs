#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

// Max binary length
#define MAX_BINARY_LENGTH 64

void clearInputBuffer() {
    int c;

    while ((c = getchar()) != '\n' && c != EOF);
}

// Validate if a string only contains ones and zeroes
int validateBinary(const char *binary) {
    if (binary == NULL || strlen(binary) == 0) {
        return 0;
    }

    // Check for too long strings
    if (strlen(binary) > MAX_BINARY_LENGTH) {
        printf("\nError, the binary number was too long (max %d digits).\n", MAX_BINARY_LENGTH);
        return 0;
    }

    // Iterate the string
    for (int i = 0; i < strlen(binary); i++) {
        if (binary[i] != '0' && binary[i] != '1') {
            return 0; // Invalid digit found
        }
    }

    return 1;
}

// Convert a binary string to decimal
long long binaryToDecimal(const char* binary) {
    long long decimal = 0;
    int length = strlen(binary);

    // Process the binary
    for (int i = 0; i < length; i++) {
        int bit = binary[i] - '0';

        // Calculate position from the right (example, "1011", leftmost 1 is at pos. 3, 2^3)
        int position = length - 1 - i;

        // Decimal result, formula: bit * (2^pos)
        decimal += bit * (long long)pow(2, position);
    }

    return decimal;
}

// Alternate method using bit shifting
long long binaryToDecimalAlternative(const char *binary) {
    long long decimal = 0;
    int length = strlen(binary);

    // Process binary from left to right
    for (int i = 0; i < length; i++) {
        // Shift existing decimal value left by 1 bit
        decimal = decimal << 1;

        // Add the current bit, if its '1' add 1 to decimal
        if (binary[i] == '1') {
            decimal += 1;
        }
        // If the current bit is '0' do nothing
    }

    return decimal;
}

void displayBinaryBreakdown(const char *binary) {
    int length = strlen(binary);

    printf("\nStep by step conversion:\n");
    
    for (int i = 0; i < length; i++) {
        int bit = binary[i] - '0';
        int position = length - 1 - i;
        long long contribution = bit * (long long)pow(2, position);

        printf("Position %d: %c * 2^%d = %c * %lld = %lld\n",
                i, binary[i], position, binary[i],
            (long long)pow(2, position), contribution);
    }
}

int getUserChoice() {
    char choice[10];  // User input buffer

    printf("\nDo you want to convert another binary number? (y/n): ");

    if (scanf("%9s", choice) != 1) {
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();

    char firstChar = tolower(choice[0]);

    if (firstChar == 'y') {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    char binary[MAX_BINARY_LENGTH + 1];
    int continueProgram = 1;

    printf("Binary to decimal converter\n\n");

    while (continueProgram) {
        printf("Enter a binary number (up to %d bits): ", MAX_BINARY_LENGTH);

        if (scanf("%64s", binary) != 1) {
            printf("\nError, failed to read input!\n");
            clearInputBuffer();

            if (!getUserChoice()) {
                break;
            }

            continue;
        }

        clearInputBuffer();

        // Check for too long inputs
        if (strlen(binary) >= MAX_BINARY_LENGTH) {
            printf("\nWarning: Input may have been truncated to %d characters.\n", MAX_BINARY_LENGTH);
        }

        // Input validation
        if (!validateBinary(binary)) {
            printf("\nError: Invalid binary number!\n");
            printf("Binary numbers can only contain 1 and 0.\n");

            // Reprompt user
            if (!getUserChoice()) {
                break;
            }
            continue;
        }

        if (strlen(binary) == 0) {
            printf("\nError, empty input!\n");

            if (!getUserChoice()) {
                break;
            }
            continue;
        }

        long long decimalMethod1 = binaryToDecimal(binary);
        long long decimalMethod2 = binaryToDecimalAlternative(binary);

        printf("----------------------\n");
        printf("Results:\n");
        printf("Binary: %s\n", binary);
        printf("Decimal: %lld\n", decimalMethod1);

        // Show detailed breakdown
        if (strlen(binary) <= 16) {
            displayBinaryBreakdown(binary);
        } else {
            printf("\n(Binary number too long for detailed breakdown)\n");
        }

        printf("\n");
        printf("Verification: Both methods give same result? %s\n",
                (decimalMethod1 == decimalMethod2) ? "YES" : "NO");

        printf("\n------------------------------------------------\n");

        continueProgram = getUserChoice();
    }

    printf("\nThanks for using the binary to decimal converter!\n");
    printf("Bye!\n");

    return 0;
}