#include <stdio.h>
#include <ctype.h>

int main() {
    char str[] = "Hello World";
    char *p = str;
    int count = 0;

    while (*p != '\0') {
        char ch = tolower(*p);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
            count++;
        p++;
    }

    printf("Number of vowels: %d\n", count);

    return 0;
}