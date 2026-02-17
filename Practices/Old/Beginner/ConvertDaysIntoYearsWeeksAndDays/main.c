#include <stdio.h>

int main() {

    int total_days = 0;
    int years, weeks, days = 0;

    printf("Enter the number of days: ");
    scanf("%d", &total_days);

    years = total_days / 365;
    weeks = (total_days % 365) / 7;
    days = (total_days % 365) % 7;

    printf("%d days = %d year(s), %d week(s), and %d day(s)\n", total_days, years, weeks, days);

    return 0;
}