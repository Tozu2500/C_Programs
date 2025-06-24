#include "student.h"

int add_grade(StudentDatabase *db, int student_id, const char *subject, float grade, int credits) {
    if (!db || !subject) return 0;
    
    Student *student = find_student_by_id(db, student_id);
    if (!student) {
        printf("Error: Student with ID %d not found\n", student_id);
        return 0;
    }
    
    if (student->num_grades >= MAX_SUBJECTS) {
        printf("Error: Maximum number of subjects reached for student\n");
        return 0;
    }
    
    if (grade < 0.0 || grade > 100.0) {
        printf("Error: Grade must be between 0.0 and 100.0\n");
        return 0;
    }
    
    if (credits <= 0) {
        printf("Error: Credits must be positive\n");
        return 0;
    }
    
    // Check if subject already exists
    for (int i = 0; i < student->num_grades; i++) {
        if (strcasecmp(student->grades[i].subject, subject) == 0) {
            printf("Error: Grade for subject '%s' already exists. Use update_grade to modify.\n", subject);
            return 0;
        }
    }
    
    // Add new grade
    strncpy(student->grades[student->num_grades].subject, subject, MAX_COURSE_LENGTH - 1);
    student->grades[student->num_grades].subject[MAX_COURSE_LENGTH - 1] = '\0';
    student->grades[student->num_grades].grade = grade;
    student->grades[student->num_grades].credits = credits;
    student->num_grades++;
    
    // Update GPA
    update_student_gpa(student);
    
    printf("Grade added successfully for %s: %.2f (%d credits)\n", subject, grade, credits);
    return 1;
}

int update_grade(StudentDatabase *db, int student_id, const char *subject, float new_grade) {
    if (!db || !subject) return 0;
    
    Student *student = find_student_by_id(db, student_id);
    if (!student) {
        printf("Error: Student with ID %d not found\n", student_id);
        return 0;
    }
    
    if (new_grade < 0.0 || new_grade > 100.0) {
        printf("Error: Grade must be between 0.0 and 100.0\n");
        return 0;
    }
    
    // Find and update the grade
    for (int i = 0; i < student->num_grades; i++) {
        if (strcasecmp(student->grades[i].subject, subject) == 0) {
            float old_grade = student->grades[i].grade;
            student->grades[i].grade = new_grade;
            update_student_gpa(student);
            printf("Grade updated for %s: %.2f -> %.2f\n", subject, old_grade, new_grade);
            return 1;
        }
    }
    
    printf("Error: Grade for subject '%s' not found\n", subject);
    return 0;
}

int delete_grade(StudentDatabase *db, int student_id, const char *subject) {
    if (!db || !subject) return 0;
    
    Student *student = find_student_by_id(db, student_id);
    if (!student) {
        printf("Error: Student with ID %d not found\n", student_id);
        return 0;
    }
    
    // Find and delete the grade
    for (int i = 0; i < student->num_grades; i++) {
        if (strcasecmp(student->grades[i].subject, subject) == 0) {
            // Shift grades down
            for (int j = i; j < student->num_grades - 1; j++) {
                student->grades[j] = student->grades[j + 1];
            }
            student->num_grades--;
            update_student_gpa(student);
            printf("Grade for %s deleted successfully\n", subject);
            return 1;
        }
    }
    
    printf("Error: Grade for subject '%s' not found\n", subject);
    return 0;
}

float calculate_gpa(Student *student) {
    if (!student || student->num_grades == 0) return 0.0;
    
    float total_points = 0.0;
    int total_credits = 0;
    
    for (int i = 0; i < student->num_grades; i++) {
        // Convert percentage to 4.0 scale
        float grade_points;
        if (student->grades[i].grade >= 90.0) grade_points = 4.0;
        else if (student->grades[i].grade >= 87.0) grade_points = 3.7;
        else if (student->grades[i].grade >= 83.0) grade_points = 3.3;
        else if (student->grades[i].grade >= 80.0) grade_points = 3.0;
        else if (student->grades[i].grade >= 77.0) grade_points = 2.7;
        else if (student->grades[i].grade >= 73.0) grade_points = 2.3;
        else if (student->grades[i].grade >= 70.0) grade_points = 2.0;
        else if (student->grades[i].grade >= 67.0) grade_points = 1.7;
        else if (student->grades[i].grade >= 65.0) grade_points = 1.3;
        else if (student->grades[i].grade >= 60.0) grade_points = 1.0;
        else grade_points = 0.0;
        
        total_points += grade_points * student->grades[i].credits;
        total_credits += student->grades[i].credits;
    }
    
    return total_credits > 0 ? total_points / total_credits : 0.0;
}

void update_student_gpa(Student *student) {
    if (student) {
        student->gpa = calculate_gpa(student);
    }
}

void display_student_grades(const Student *student) {
    if (!student) return;
    
    printf("\n=== Grades for %s %s (ID: %d) ===\n", 
           student->first_name, student->last_name, student->id);
    
    if (student->num_grades == 0) {
        printf("No grades recorded.\n");
        return;
    }
    
    printf("%-20s %-8s %-8s\n", "Subject", "Grade", "Credits");
    printf("%-20s %-8s %-8s\n", "--------------------", "--------", "--------");
    
    for (int i = 0; i < student->num_grades; i++) {
        printf("%-20s %-8.2f %-8d\n", 
               student->grades[i].subject,
               student->grades[i].grade,
               student->grades[i].credits);
    }
    
    printf("\nGPA: %.2f\n", student->gpa);
}

void generate_transcript(const Student *student) {
    if (!student) return;
    
    printf("\n");
    printf("=====================================\n");
    printf("         OFFICIAL TRANSCRIPT         \n");
    printf("=====================================\n");
    printf("Student ID: %d\n", student->id);
    printf("Name: %s %s\n", student->first_name, student->last_name);
    printf("Course: %s\n", student->course);
    printf("Enrollment Year: %d\n", student->enrollment_year);
    printf("Email: %s\n", student->email);
    printf("-------------------------------------\n");
    
    if (student->num_grades == 0) {
        printf("No grades recorded.\n");
    } else {
        printf("%-25s %-8s %-8s %-8s\n", "Subject", "Grade", "Credits", "Points");
        printf("%-25s %-8s %-8s %-8s\n", 
               "-------------------------", "--------", "--------", "--------");
        
        int total_credits = 0;
        for (int i = 0; i < student->num_grades; i++) {
            float grade_points;
            if (student->grades[i].grade >= 90.0) grade_points = 4.0;
            else if (student->grades[i].grade >= 87.0) grade_points = 3.7;
            else if (student->grades[i].grade >= 83.0) grade_points = 3.3;
            else if (student->grades[i].grade >= 80.0) grade_points = 3.0;
            else if (student->grades[i].grade >= 77.0) grade_points = 2.7;
            else if (student->grades[i].grade >= 73.0) grade_points = 2.3;
            else if (student->grades[i].grade >= 70.0) grade_points = 2.0;
            else if (student->grades[i].grade >= 67.0) grade_points = 1.7;
            else if (student->grades[i].grade >= 65.0) grade_points = 1.3;
            else if (student->grades[i].grade >= 60.0) grade_points = 1.0;
            else grade_points = 0.0;
            
            printf("%-25s %-8.2f %-8d %-8.2f\n",
                   student->grades[i].subject,
                   student->grades[i].grade,
                   student->grades[i].credits,
                   grade_points);
            
            total_credits += student->grades[i].credits;
        }
        
        printf("-------------------------------------\n");
        printf("Total Credits: %d\n", total_credits);
        printf("Cumulative GPA: %.2f\n", student->gpa);
    }
    
    printf("=====================================\n");
    printf("Generated on: ");
    time_t now = time(NULL);
    printf("%s", ctime(&now));
    printf("=====================================\n\n");
}