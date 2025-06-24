#include "student.h"

int init_database(StudentDatabase *db) {
    if (!db) return 0;
    
    db->count = 0;
    db->next_id = 1;
    memset(db->students, 0, sizeof(db->students));
    
    return 1;
}

int save_database(StudentDatabase *db, const char *filename) {
    if (!db || !filename) return 0;
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s for writing\n", filename);
        return 0;
    }
    
    // Write database header
    fwrite(&db->count, sizeof(int), 1, file);
    fwrite(&db->next_id, sizeof(int), 1, file);
    
    // Write all students
    for (int i = 0; i < db->count; i++) {
        fwrite(&db->students[i], sizeof(Student), 1, file);
    }
    
    fclose(file);
    printf("Database saved successfully to %s\n", filename);
    return 1;
}

int load_database(StudentDatabase *db, const char *filename) {
    if (!db || !filename) return 0;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Warning: Could not open file %s for reading. Starting with empty database.\n", filename);
        return init_database(db);
    }
    
    // Read database header
    if (fread(&db->count, sizeof(int), 1, file) != 1 ||
        fread(&db->next_id, sizeof(int), 1, file) != 1) {
        printf("Error: Invalid database file format\n");
        fclose(file);
        return init_database(db);
    }
    
    // Validate count
    if (db->count < 0 || db->count > MAX_STUDENTS) {
        printf("Error: Invalid student count in database\n");
        fclose(file);
        return init_database(db);
    }
    
    // Read all students
    for (int i = 0; i < db->count; i++) {
        if (fread(&db->students[i], sizeof(Student), 1, file) != 1) {
            printf("Error: Could not read student data\n");
            fclose(file);
            return 0;
        }
    }
    
    fclose(file);
    printf("Database loaded successfully from %s (%d students)\n", filename, db->count);
    return 1;
}

void cleanup_database(StudentDatabase *db) {
    if (db) {
        memset(db, 0, sizeof(StudentDatabase));
    }
}

int add_student(StudentDatabase *db, Student *student) {
    if (!db || !student) return 0;
    
    if (db->count >= MAX_STUDENTS) {
        printf("Error: Database is full (maximum %d students)\n", MAX_STUDENTS);
        return 0;
    }
    
    // Assign ID and set defaults
    student->id = db->next_id++;
    student->is_active = 1;
    student->num_grades = 0;
    student->gpa = 0.0;
    
    // Copy student to database
    db->students[db->count] = *student;
    db->count++;
    
    printf("Student added successfully with ID: %d\n", student->id);
    return student->id;
}

int update_student(StudentDatabase *db, int id, Student *updated_student) {
    if (!db || !updated_student) return 0;
    
    Student *student = find_student_by_id(db, id);
    if (!student) {
        printf("Error: Student with ID %d not found\n", id);
        return 0;
    }
    
    // Preserve ID, grades, and GPA
    int original_id = student->id;
    Grade original_grades[MAX_SUBJECTS];
    int original_num_grades = student->num_grades;
    float original_gpa = student->gpa;
    
    memcpy(original_grades, student->grades, sizeof(original_grades));
    
    // Update student data
    *student = *updated_student;
    student->id = original_id;
    student->num_grades = original_num_grades;
    student->gpa = original_gpa;
    memcpy(student->grades, original_grades, sizeof(original_grades));
    
    printf("Student with ID %d updated successfully\n", id);
    return 1;
}

int delete_student(StudentDatabase *db, int id) {
    if (!db) return 0;
    
    int index = -1;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("Error: Student with ID %d not found\n", id);
        return 0;
    }
    
    // Shift students down
    for (int i = index; i < db->count - 1; i++) {
        db->students[i] = db->students[i + 1];
    }
    
    db->count--;
    printf("Student with ID %d deleted successfully\n", id);
    return 1;
}

Student* find_student_by_id(StudentDatabase *db, int id) {
    if (!db) return NULL;
    
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].id == id) {
            return &db->students[i];
        }
    }
    
    return NULL;
}

Student* find_student_by_name(StudentDatabase *db, const char *first_name, const char *last_name) {
    if (!db || !first_name || !last_name) return NULL;
    
    for (int i = 0; i < db->count; i++) {
        if (strcasecmp(db->students[i].first_name, first_name) == 0 &&
            strcasecmp(db->students[i].last_name, last_name) == 0) {
            return &db->students[i];
        }
    }
    
    return NULL;
}