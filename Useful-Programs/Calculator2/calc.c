#include <stdio.h>
#include <stdlib.h>

// Function for the display menu
void displayMenu() {
    printf("\n --- Simple Calculator 2 --- \n");
    printf("1. Addition\n");
    printf("2. Subtraction\n");
    printf("3. Multiplication\n");
    printf("4. Division\n");
    printf("5. Exit\n");
    printf("Choose an option (1-5): ");
}

// Function for addition
double add (double a, double b) {
    return a + b;
}

// Function for subtraction
double subtract(double a, double b) {
    return a - b;
}

// Function for multiply
double multiply(double a, double b) {
    return a * b;
}

// Function for division
double divide(double a, double b) {
    if (b != 0) {
        return a / b;
    } else {
        printf("Error! You can't divide by zero!\n");
        return 0;
    }
}

int main() {

    int choice = 0;

    double num1 = 0;
    double num2 = 0;
    double result = 0;

    printf("Welcome to the calculator 2\n");

    while(1) {
        displayMenu();
        scanf("%d", &choice);

        if (choice == 5) {
            printf("Thank you for using the calculator!\n");
            break;
        }

        if (choice >= 1 && choice <= 4) {
            printf("Enter the first number: ");
            scanf("%lf", &num1);
            printf("Enter the second number: ");
            scanf("%lf", &num2);

            switch (choice) {
                case 1:
                    result = add(num1, num2);
                    printf("Result: %.2f + %.2f = %.2f\n", num1, num2, result);
                    break;
                case 2:
                    result = subtract(num1, num2);
                    printf("Result: %.2f - %.2f = %.2f\n", num1, num2, result);
                    break;
                case 3:
                    result = multiply(num1, num2);
                    printf("Result %.2f * %.2f = %.2f\n", num1, num2, result);
                    break;
                case 4:
                    result = divide(num1, num2);
                    if (num2 != 0) {
                        printf("Result: %.2f / %.2f = %.2f", num1, num2, result);
                    }
                    break;
            }
        } else {
            printf("Invalid choice! Please select 1-5\n");
        }
    }

    return 0;
}