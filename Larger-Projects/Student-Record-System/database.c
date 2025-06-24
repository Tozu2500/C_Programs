#include "student.h"

int init_database(StudentDatabase *db) {
    if (!db) {
        return 0;
    }

    db->count = 0;
    db->next_id = 1;
    memset(db->students, 0, sizeof(db->students));

    return 1;
}

int save_database(StudentDatabase *db, const char *filename) {
    if (!db || !filename) {
        return 0;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return 0;
    }

    fwrite(&db->count, sizeof(int), 1, file);
    fwrite(&db->next_id, sizeof(int), 1, file);

    for (int i = 0; i < db->count; i++) {
        fwrite(&db->students[i], sizeof(Student), 1, file);
    }

    fclose(file);
    printf("Database successfully saved to %s\n", filename);
    return 1;
}

int load_database(StudentDatabase *db, const char *filename) {
    if (!db || !filename) {
        return 0;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Warning: Could not open the file %s for reading. It might not exist. Starting with an empty database.\n", filename);
        return init_database(db);
    }

    if (fread(&db->count, sizeof(int), 1, file) != 1 ||
        fread(&db->next_id, sizeof(int), 1, file) != 1) {
            printf("Error: Invalid database file format. Starting with an empty database.\n");
            fclose(file);
            return init_database(db);
        }

    if (db->count < 0 || db->count > MAX_STUDENTS) {
        printf("Error: Invalid student count in the database, must be between 1-1000.\n");
        fclose(file);
        return init_database(db);
    }

    for (int i = 0; i < db->count; i++) {
        if (fread(&db->students[i], sizeof(Student), 1, file) != 1) {
            printf("Error: Couldn't read student data.\n");
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    printf("Database loaded successfully from %s (with %d students)\n", filename, db->count);
    return 1;
}

void cleanup_database(StudentDatabase *db) {
    if (db) {
        memset(db, 0, sizeof(StudentDatabase));
    }
}

int add_student(StudentDatabase *db, Student *student) {
    if (!db || !student) {
        return 0;
    }

    if (db->count >= MAX_STUDENTS) {
        printf("Error: Database is full (Maximum %d students)\n", MAX_STUDENTS);
        return 0;
    }

    student->id = db->next_id++;
    student->is_active = 1;
    student->num_grades = 0;
    student->gpa = 0.0;

    db->students[db->count] = *student;
    db->count++;

    printf("Student added successfully with an ID of %d\n", student->id);
    return student->id;
}

int update_student(StudentDatabase *db, int id, Student *updated_student) {
    if (!db || !updated_student) {
        return 0;
    }

    Student *student = find_student_by_id(db, id);
    if (!student) {
        printf("Error: Student with ID %d not found\n", id);
        return 0;
    }

    int original_id = student->id;
    Grade original_grades[MAX_SUBJECTS];
    int original_num_grades = student->num_grades;
    float original_gpa = student->gpa;

    memcpy(original_grades, student->grades, sizeof(original_grades));

    *student = *updated_student;
    student->id = original_id;
    student->num_grades = original_num_grades;
    student->gpa = original_gpa;
    memcpy(student->grades, original_grades, sizeof(original_grades));

    printf("Student with ID %d was updated successfully\n", id);
    return 1;
}

int delete_student(StudentDatabase *db, int id) {
    if (!db) {
        return 0;
    }

    int index = -1;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Error: Student with an ID of %d was not found\n", id);
        return 0;
    }

    for (int i = index; i < db->count - 1; i++) {
        db->students[i] = db->students[i + 1];
    }

    db->count--;
    printf("Student with an ID of %d was deleted successfully!\n", id);
    return 1;
}

Student* find_student_by_id(StudentDatabase *db, int id) {
    if (!db) {
        return NULL;
    }

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].id == id) {
            return &db->students[i];
        }
    }

    return NULL;
}

Student* find_student_by_name(StudentDatabase *db, const char *first_name, const char *last_name) {
    if (!db || !first_name || !last_name) {
        return NULL;
    }

    for (int i = 0; i < db->count; i++) {
        if (strcasecmp(db->students[i].first_name, first_name) == 0 &&
            strcasemp(db->students[i].last_name, last_name) == 0) {
                return &db->students[i];
            }
    }

    return NULL;
}