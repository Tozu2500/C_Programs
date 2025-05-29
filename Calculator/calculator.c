#include <stdio.h>

int main() {

    char operator = '\0';
    double num1 = 0;
    double num2 = 0;
    double result = 0;

    printf("Calculator, choose your operator (+, -, *, /): \n");
    scanf(" %c", &operator);

    printf("Enter two numbers: \n");
    scanf("%lf %lf", &num1, &num2);

    switch(operator) {
        case '+':
            result = num1 + num2;
            printf("Result: %.2lf\n", result);
            break;
        case '-':
            result = num1 - num2;
            printf("Result: %.2lf\n", result);
            break;
        case '*':
            result = num1 * num2;
            printf("Result: %.2lf\n", result);
            break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
                printf("Result %.2lf\n", result);
            } else {
                printf("You cannot divide by zero!\n");
            }
            break;
        default:
            printf("Invalid operator!\n");
    }

    return 0;
}