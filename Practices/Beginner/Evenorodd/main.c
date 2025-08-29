#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main() {
    char input[100];
    int number;
    double doubleNumber;

    printf("Welcome to the 'Even or Odd number checker program'!\n");
    printf("Enter a number (integer or decimal) and we'll find out!\n");

    // Read input as string
    scanf("%99s", input);

    // Check if input contains a dot (.)
    if (strchr(input, '.') != NULL) {
        // It's a double
        doubleNumber = atof(input);

        printf("You have entered a decimal number! We'll round it to the nearest whole number!\n");
        number = (int)round(doubleNumber);
    } else {
        // It's an integer
        number = atoi(input);
    }

    // Even or odd check
    if (number % 2 == 0) {
        printf("The number %d is even!\n", number);
    } else {
        printf("The number %d is odd!\n", number);
    }

    return 0;
}
