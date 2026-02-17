#include <stdio.h>
#include <stdbool.h>

int main() {

    int age = 20;
    printf("The listed 'age' is: %d\n", age);

    float amount = 6.433;
    printf("The listed 'amount' is %f\n", amount);

    double doubleAmount = 12.4854358434;
    printf("The listed 'doubleAmount' is %lf, it can store more decimals than a float!\n", doubleAmount);

    char grade = 'A';
    printf("The listed char of 'grade' is %c\n", grade);

    char characterArray[] = "String of text";
    printf("The 'characterArray' is %s\n", characterArray);

    short smallNum = 23838;
    printf("The 'smallNum' is a short: %hd\n", smallNum);

    long longNum = 43295823235435L;
    printf("The 'longNum' is a long: %ld\n", longNum);

    long long longLongNum = 324893742389479472LL;
    printf("The 'longLongNum' is a long long: %lld\n", longLongNum);

    unsigned int unsignedInt = 42;
    printf("Unsigned int: %u\n", unsignedInt);

    unsigned long unsignedLong = 848484UL;
    printf("Unsigned long: %lu\n", unsignedLong);
    
    bool isActive = true;
    printf("bool (boolean) 'isActive': %d\n", isActive);

    return 0;
}