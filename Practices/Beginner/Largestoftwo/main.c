#include <stdio.h>

int main() {

    int num1 = 0;
    int num2 = 0;
    printf("Please enter the first number: \n");

    scanf("%d", &num1);

    printf("Please enter the second number: \n");
    scanf("%d", &num2);

    if (num1 > num2) {
        printf("Number one %d was larger than number two!", num1, num2);
    } else if (num1 < num2) {
        printf("Number one %d was smaller than number two!", num1, num2);
    } else {
        printf("The numbers we're equal!");
    }

    return 0;
}