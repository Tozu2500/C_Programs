#include "student.h"

int search_students_by_course(StudentDatabase *db, const char *course, Student results[], int max_results) {
    if (!db || !course || !results) return 0;
    
    int count = 0;
    for (int i = 0; i < db->count && count < max_results; i++) {
        if (strcasecmp(db->students[i].course, course) == 0 && db->students[i].is_active) {
            results[count] = db->students[i];
            count++;
        }
    }
    
    return count;
}

int search_students_by_year(StudentDatabase *db, int year, Student results[], int max_results) {
    if (!db || !results) return 0;
    
    int count = 0;
    for (int i = 0; i < db->count && count < max_results; i++) {
        if (db->students[i].enrollment_year == year && db->students[i].is_active) {
            results[count] = db->students[i];
            count++;
        }
    }
    
    return count;
}

int get_students_by_gpa_range(StudentDatabase *db, float min_gpa, float max_gpa, Student results[], int max_results) {
    if (!db || !results) return 0;
    
    int count = 0;
    for (int i = 0; i < db->count && count < max_results; i++) {
        if (db->students[i].gpa >= min_gpa && db->students[i].gpa <= max_gpa && db->students[i].is_active) {
            results[count] = db->students[i];
            count++;
        }
    }
    
    return count;
}

void sort_students_by_name(Student students[], int count) {
    if (!students || count <= 1) return;
    
    // Simple bubble sort by last name, then first name
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int cmp = strcasecmp(students[j].last_name, students[j + 1].last_name);
            if (cmp > 0 || (cmp == 0 && strcasecmp(students[j].first_name, students[j + 1].first_name) > 0)) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_students_by_id(Student students[], int count) {
    if (!students || count <= 1) return;
    
    // Simple bubble sort by ID
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (students[j].id > students[j + 1].id) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void sort_students_by_gpa(Student students[], int count) {
    if (!students || count <= 1) return;
    
    // Simple bubble sort by GPA (descending order)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (students[j].gpa < students[j + 1].gpa) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void generate_class_report(StudentDatabase *db, const char *course) {
    if (!db || !course) return;
    
    Student students[MAX_STUDENTS];
    int count = search_students_by_course(db, course, students, MAX_STUDENTS);
    
    if (count == 0) {
        printf("No students found for course: %s\n", course);
        return;
    }
    
    // Sort by GPA for better presentation
    sort_students_by_gpa(students, count);
    
    printf("\n");
    printf("=========================================\n");
    printf("        CLASS REPORT: %s\n", course);
    printf("=========================================\n");
    printf("Total Students: %d\n", count);
    
    // Calculate class statistics
    float total_gpa = 0.0;
    float highest_gpa = 0.0;
    float lowest_gpa = 4.0;
    int students_with_grades = 0;
    
    for (int i = 0; i < count; i++) {
        if (students[i].num_grades > 0) {
            total_gpa += students[i].gpa;
            students_with_grades++;
            if (students[i].gpa > highest_gpa) highest_gpa = students[i].gpa;
            if (students[i].gpa < lowest_gpa) lowest_gpa = students[i].gpa;
        }
    }
    
    if (students_with_grades > 0) {
        printf("Average GPA: %.2f\n", total_gpa / students_with_grades);
        printf("Highest GPA: %.2f\n", highest_gpa);
        printf("Lowest GPA: %.2f\n", lowest_gpa);
    } else {
        printf("No grades recorded for any students in this course.\n");
    }
    
    printf("-----------------------------------------\n");
    printf("%-5s %-20s %-20s %-8s %-6s\n", 
           "ID", "First Name", "Last Name", "GPA", "Year");
    printf("%-5s %-20s %-20s %-8s %-6s\n", 
           "-----", "--------------------", "--------------------", "--------", "------");
    
    for (int i = 0; i < count; i++) {
        printf("%-5d %-20s %-20s %-8.2f %-6d\n",
               students[i].id,
               students[i].first_name,
               students[i].last_name,
               students[i].gpa,
               students[i].enrollment_year);
    }
    
    printf("=========================================\n\n");
}

// Advanced search function
int advanced_search(StudentDatabase *db, const char *search_term, Student results[], int max_results) {
    if (!db || !search_term || !results) return 0;
    
    int count = 0;
    for (int i = 0; i < db->count && count < max_results; i++) {
        if (!db->students[i].is_active) continue;
        
        // Search in multiple fields
        if (strstr(db->students[i].first_name, search_term) ||
            strstr(db->students[i].last_name, search_term) ||
            strstr(db->students[i].email, search_term) ||
            strstr(db->students[i].course, search_term) ||
            strstr(db->students[i].phone, search_term)) {
            results[count] = db->students[i];
            count++;
        }
    }
    
    return count;
}