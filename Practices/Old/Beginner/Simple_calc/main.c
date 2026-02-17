#include <stdio.h>

int main() {

    // Variables to store the numbers
    double num1 = 0.0;
    double num2 = 0.0;

    printf("Enter two numbers, and I will return their sum, difference, product and quotient\n");

    // Read the users input
    scanf("%lf %lf", &num1, &num2);

    // Calculations
    printf("Sum: %.2lf\n", num1 + num2);
    printf("Difference: %.2lf\n", num1 - num2);
    printf("Product: %.2lf\n", num1 * num2);

    // Handle zero division carefully
    if (num2 != 0) {
        printf("Quotient: %.2lf\n", num1 / num2);
    } else {
        printf("You can't divide by zero!");
    }

    return 0;
}