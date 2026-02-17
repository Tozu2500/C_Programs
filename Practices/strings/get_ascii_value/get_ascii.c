#include <stdio.h>

int main() {

    char ch;
    int asciiValue;

    printf("Enter a character: \n");
    scanf("%c", &ch);

    // Implicit conversion from char to int
    asciiValue = ch;

    printf("The ASCII value of '%c' is %d\n", ch, asciiValue);

    return 0;
}