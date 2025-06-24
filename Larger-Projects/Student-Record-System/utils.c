#include "student.h"
#include <ctype.h>

int validate_email(const char *email) {
    if (!email || strlen(email) < 5) return 0;
    
    // Basic email validation
    int at_count = 0;
    int dot_after_at = 0;
    int at_position = -1;
    
    for (int i = 0; email[i]; i++) {
        if (email[i] == '@') {
            at_count++;
            at_position = i;
        } else if (email[i] == '.' && at_position != -1 && i > at_position) {
            dot_after_at = 1;
        }
    }
    
    return (at_count == 1 && dot_after_at && at_position > 0 && 
            at_position < (int)strlen(email) - 2);
}

int validate_phone(const char *phone) {
    if (!phone) return 0;
    
    int digit_count = 0;
    for (int i = 0; phone[i]; i++) {
        if (isdigit(phone[i])) {
            digit_count++;
        } else if (phone[i] != ' ' && phone[i] != '-' && phone[i] != '(' && 
                   phone[i] != ')' && phone[i] != '+') {
            return 0; // Invalid character
        }
    }
    
    return digit_count >= 10 && digit_count <= 15;
}

void trim_whitespace(char *str) {
    if (!str) return;
    
    // Trim leading whitespace
    int start = 0;
    while (str[start] && isspace(str[start])) {
        start++;
    }
    
    if (start > 0) {
        memmove(str, str + start, strlen(str + start) + 1);
    }
    
    // Trim trailing whitespace
    int end = strlen(str) - 1;
    while (end >= 0 && isspace(str[end])) {
        str[end] = '\0';
        end--;
    }
}

int get_current_year(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return tm_info->tm_year + 1900;
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_student_input(Student *student) {
    if (!student) return 0;
    
    printf("Enter student information:\n");
    
    printf("First Name: ");
    if (!fgets(student->first_name, MAX_NAME_LENGTH, stdin)) return 0;
    trim_whitespace(student->first_name);
    if (strlen(student->first_name) == 0) {
        printf("Error: First name cannot be empty\n");
        return 0;
    }
    
    printf("Last Name: ");
    if (!fgets(student->last_name, MAX_NAME_LENGTH, stdin)) return 0;
    trim_whitespace(student->last_name);
    if (strlen(student->last_name) == 0) {
        printf("Error: Last name cannot be empty\n");
        return 0;
    }
    
    printf("Email: ");
    if (!fgets(student->email, MAX_EMAIL_LENGTH, stdin)) return 0;
    trim_whitespace(student->email);
    if (!validate_email(student->email)) {
        printf("Error: Invalid email format\n");
        return 0;
    }
    
    printf("Phone: ");
    if (!fgets(student->phone, MAX_PHONE_LENGTH, stdin)) return 0;
    trim_whitespace(student->phone);
    if (!validate_phone(student->phone)) {
        printf("Error: Invalid phone number format\n");
        return 0;
    }
    
    printf("Address: ");
    if (!fgets(student->address, MAX_ADDRESS_LENGTH, stdin)) return 0;
    trim_whitespace(student->address);
    
    printf("Course: ");
    if (!fgets(student->course, MAX_COURSE_LENGTH, stdin)) return 0;
    trim_whitespace(student->course);
    if (strlen(student->course) == 0) {
        printf("Error: Course cannot be empty\n");
        return 0;
    }
    
    printf("Enrollment Year (or 0 for current year): ");
    if (scanf("%d", &student->enrollment_year) != 1) {
        printf("Error: Invalid year format\n");
        clear_input_buffer();
        return 0;
    }
    clear_input_buffer();
    
    if (student->enrollment_year == 0) {
        student->enrollment_year = get_current_year();
    }
    
    if (student->enrollment_year < 1900 || student->enrollment_year > get_current_year() + 1) {
        printf("Error: Invalid enrollment year\n");
        return 0;
    }
    
    return 1;
}

void print_menu(void) {
    printf("\n========================================\n");
    printf("        STUDENT RECORD SYSTEM\n");
    printf("========================================\n");
    printf("1.  Add Student\n");
    printf("2.  Update Student\n");
    printf("3.  Delete Student\n");
    printf("4.  Find Student by ID\n");
    printf("5.  Find Student by Name\n");
    printf("6.  Display All Students\n");
    printf("7.  Add Grade\n");
    printf("8.  Update Grade\n");
    printf("9.  Delete Grade\n");
    printf("10. Display Student Grades\n");
    printf("11. Generate Transcript\n");
    printf("12. Search by Course\n");
    printf("13. Search by Year\n");
    printf("14. Search by GPA Range\n");
    printf("15. Generate Class Report\n");
    printf("16. Advanced Search\n");
    printf("17. Save Database\n");
    printf("18. Load Database\n");
    printf("19. Exit\n");
    printf("========================================\n");
    printf("Enter your choice: ");
}

void display_student(const Student *student) {
    if (!student) return;
    
    printf("\n=== Student Information ===\n");
    printf("ID: %d\n", student->id);
    printf("Name: %s %s\n", student->first_name, student->last_name);
    printf("Email: %s\n", student->email);
    printf("Phone: %s\n", student->phone);
    printf("Address: %s\n", student->address);
    printf("Course: %s\n", student->course);
    printf("Enrollment Year: %d\n", student->enrollment_year);
    printf("GPA: %.2f\n", student->gpa);
    printf("Number of Subjects: %d\n", student->num_grades);
    printf("Status: %s\n", student->is_active ? "Active" : "Inactive");
    printf("===========================\n");
}

void display_all_students(const StudentDatabase *db) {
    if (!db) return;
    
    if (db->count == 0) {
        printf("No students in database.\n");
        return;
    }
    
    printf("\n=== All Students ===\n");
    printf("%-5s %-15s %-15s %-25s %-20s %-8s %-6s\n", 
           "ID", "First Name", "Last Name", "Email", "Course", "GPA", "Year");
    printf("%-5s %-15s %-15s %-25s %-20s %-8s %-6s\n", 
           "-----", "---------------", "---------------", "-------------------------", 
           "--------------------", "--------", "------");
    
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            printf("%-5d %-15s %-15s %-25s %-20s %-8.2f %-6d\n",
                   db->students[i].id,
                   db->students[i].first_name,
                   db->students[i].last_name,
                   db->students[i].email,
                   db->students[i].course,
                   db->students[i].gpa,
                   db->students[i].enrollment_year);
        }
    }
    printf("====================\n");
    printf("Total Students: %d\n", db->count);
}