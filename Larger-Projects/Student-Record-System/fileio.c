#include <stdio.h>
#include "fileio.h"
#include "student.h"

int loadStudents(Student students[]) {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No existing data was found. Starting with an empty database.\n");
        return 0;
    }

    int count = 0;
    while (fread(&students[count], sizeof(Student), 1, file) == 1 && count < MAX_STUDENTS) {
        count++;
    }

    fclose(file);
    printf("Loaded %d students from file.\n", count);
    return count;
}

void saveStudents(Student students[], int count) {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Couldn't save the data to a file!\n");
        return;
    }

    int written = fwrite(students, sizeof(Student), count, file);
    fclose(file);

    if (written == count) {
        printf("Successfully saved %d students to the file.\n", count);
    } else {
        printf("Error: Couldn't save all of the students to the file!\n");
    }
}