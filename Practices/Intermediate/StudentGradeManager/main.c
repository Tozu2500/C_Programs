#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 50
#define NAME_LEN 50

typedef struct {
    char name[NAME_LEN];
    char grades[5];
    float average;
} Student;

void calculateAverage(Student *s) {
    int sum = 0;
    for (int i = 0; i < 5; i++) sum += s->grades[i];
    s->average = sum / 5.0;
}

void saveToFile(Student students[], int count) {
    FILE *f = fopen("students.txt", "w");
    if (!f) return;
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s %.2f\n", students[i].name, students[i].average);
    }
    fclose(f);
}

int main() {
    Student students[MAX_STUDENTS];
    int count = 0;
    char cont = 'y';

    while (cont == 'y' && count < MAX_STUDENTS) {
        printf("Enter student name: ");
        scanf("%s", students[count].name);
        printf("Enter 5 grades: ");
        for (int i = 0; i < 5; i++) scanf("%d", &students[count].grades[i]);
        calculateAverage(&students[count]);
        count++;
        printf("Add another? (y/n): ");
        scanf(" %c", &cont);
    }

    saveToFile(students, count);
    printf("Saved %d students to a file.\n", count);
    return 0;
}