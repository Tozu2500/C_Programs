#include "student.h"

// Function prototypes for advanced search
int advanced_search(StudentDatabase *db, const char *search_term, Student results[], int max_results);

int main() {
    StudentDatabase db;
    char filename[MAX_FILENAME_LENGTH] = "students.dat";
    int choice;
    
    printf("Student Record System v1.0\n");
    printf("==========================\n");
    
    // Initialize database
    if (!init_database(&db)) {
        printf("Error: Failed to initialize database\n");
        return 1;
    }
    
    // Try to load existing database
    load_database(&db, filename);
    
    while (1) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: { // Add Student
                Student student = {0};
                if (get_student_input(&student)) {
                    add_student(&db, &student);
                }
                break;
            }
            
            case 2: { // Update Student
                int id;
                printf("Enter student ID to update: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    Student *existing = find_student_by_id(&db, id);
                    if (existing) {
                        display_student(existing);
                        printf("\nEnter new information:\n");
                        Student updated = *existing;
                        if (get_student_input(&updated)) {
                            update_student(&db, id, &updated);
                        }
                    } else {
                        printf("Student with ID %d not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 3: { // Delete Student
                int id;
                printf("Enter student ID to delete: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    Student *student = find_student_by_id(&db, id);
                    if (student) {
                        display_student(student);
                        printf("Are you sure you want to delete this student? (y/n): ");
                        char confirm;
                        if (scanf("%c", &confirm) == 1 && (confirm == 'y' || confirm == 'Y')) {
                            delete_student(&db, id);
                        } else {
                            printf("Delete cancelled.\n");
                        }
                        clear_input_buffer();
                    } else {
                        printf("Student with ID %d not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 4: { // Find Student by ID
                int id;
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    Student *student = find_student_by_id(&db, id);
                    if (student) {
                        display_student(student);
                    } else {
                        printf("Student with ID %d not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 5: { // Find Student by Name
                char first_name[MAX_NAME_LENGTH], last_name[MAX_NAME_LENGTH];
                printf("Enter first name: ");
                if (fgets(first_name, MAX_NAME_LENGTH, stdin)) {
                    trim_whitespace(first_name);
                    printf("Enter last name: ");
                    if (fgets(last_name, MAX_NAME_LENGTH, stdin)) {
                        trim_whitespace(last_name);
                        Student *student = find_student_by_name(&db, first_name, last_name);
                        if (student) {
                            display_student(student);
                        } else {
                            printf("Student '%s %s' not found.\n", first_name, last_name);
                        }
                    }
                }
                break;
            }
            
            case 6: { // Display All Students
                display_all_students(&db);
                break;
            }
            
            case 7: { // Add Grade
                int id, credits;
                float grade;
                char subject[MAX_COURSE_LENGTH];
                
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    printf("Enter subject: ");
                    if (fgets(subject, MAX_COURSE_LENGTH, stdin)) {
                        trim_whitespace(subject);
                        printf("Enter grade (0-100): ");
                        if (scanf("%f", &grade) == 1) {
                            printf("Enter credits: ");
                            if (scanf("%d", &credits) == 1) {
                                add_grade(&db, id, subject, grade, credits);
                            } else {
                                printf("Invalid credits format.\n");
                            }
                        } else {
                            printf("Invalid grade format.\n");
                        }
                        clear_input_buffer();
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 8: { // Update Grade
                int id;
                float new_grade;
                char subject[MAX_COURSE_LENGTH];
                
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    printf("Enter subject: ");
                    if (fgets(subject, MAX_COURSE_LENGTH, stdin)) {
                        trim_whitespace(subject);
                        printf("Enter new grade (0-100): ");
                        if (scanf("%f", &new_grade) == 1) {
                            update_grade(&db, id, subject, new_grade);
                        } else {
                            printf("Invalid grade format.\n");
                        }
                        clear_input_buffer();
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 9: { // Delete Grade
                int id;
                char subject[MAX_COURSE_LENGTH];
                
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    printf("Enter subject: ");
                    if (fgets(subject, MAX_COURSE_LENGTH, stdin)) {
                        trim_whitespace(subject);
                        delete_grade(&db, id, subject);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 10: { // Display Student Grades
                int id;
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    Student *student = find_student_by_id(&db, id);
                    if (student) {
                        display_student_grades(student);
                    } else {
                        printf("Student with ID %d not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 11: { // Generate Transcript
                int id;
                printf("Enter student ID: ");
                if (scanf("%d", &id) == 1) {
                    clear_input_buffer();
                    Student *student = find_student_by_id(&db, id);
                    if (student) {
                        generate_transcript(student);
                    } else {
                        printf("Student with ID %d not found.\n", id);
                    }
                } else {
                    printf("Invalid ID format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 12: { // Search by Course
                char course[MAX_COURSE_LENGTH];
                printf("Enter course name: ");
                if (fgets(course, MAX_COURSE_LENGTH, stdin)) {
                    trim_whitespace(course);
                    Student results[MAX_STUDENTS];
                    int count = search_students_by_course(&db, course, results, MAX_STUDENTS);
                    if (count > 0) {
                        printf("\nFound %d students in course '%s':\n", count, course);
                        sort_students_by_name(results, count);
                        for (int i = 0; i < count; i++) {
                            printf("%d. %s %s (ID: %d, GPA: %.2f)\n", 
                                   i + 1, results[i].first_name, results[i].last_name, 
                                   results[i].id, results[i].gpa);
                        }
                    } else {
                        printf("No students found in course '%s'.\n", course);
                    }
                }
                break;
            }
            
            case 13: { // Search by Year
                int year;
                printf("Enter enrollment year: ");
                if (scanf("%d", &year) == 1) {
                    clear_input_buffer();
                    Student results[MAX_STUDENTS];
                    int count = search_students_by_year(&db, year, results, MAX_STUDENTS);
                    if (count > 0) {
                        printf("\nFound %d students enrolled in %d:\n", count, year);
                        sort_students_by_name(results, count);
                        for (int i = 0; i < count; i++) {
                            printf("%d. %s %s (ID: %d, Course: %s, GPA: %.2f)\n", 
                                   i + 1, results[i].first_name, results[i].last_name, 
                                   results[i].id, results[i].course, results[i].gpa);
                        }
                    } else {
                        printf("No students found enrolled in %d.\n", year);
                    }
                } else {
                    printf("Invalid year format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 14: { // Search by GPA Range
                float min_gpa, max_gpa;
                printf("Enter minimum GPA: ");
                if (scanf("%f", &min_gpa) == 1) {
                    printf("Enter maximum GPA: ");
                    if (scanf("%f", &max_gpa) == 1) {
                        clear_input_buffer();
                        Student results[MAX_STUDENTS];
                        int count = get_students_by_gpa_range(&db, min_gpa, max_gpa, results, MAX_STUDENTS);
                        if (count > 0) {
                            printf("\nFound %d students with GPA between %.2f and %.2f:\n", 
                                   count, min_gpa, max_gpa);
                            sort_students_by_gpa(results, count);
                            for (int i = 0; i < count; i++) {
                                printf("%d. %s %s (ID: %d, GPA: %.2f, Course: %s)\n", 
                                       i + 1, results[i].first_name, results[i].last_name, 
                                       results[i].id, results[i].gpa, results[i].course);
                            }
                        } else {
                            printf("No students found with GPA between %.2f and %.2f.\n", 
                                   min_gpa, max_gpa);
                        }
                    } else {
                        printf("Invalid maximum GPA format.\n");
                        clear_input_buffer();
                    }
                } else {
                    printf("Invalid minimum GPA format.\n");
                    clear_input_buffer();
                }
                break;
            }
            
            case 15: { // Generate Class Report
                char course[MAX_COURSE_LENGTH];
                printf("Enter course name: ");
                if (fgets(course, MAX_COURSE_LENGTH, stdin)) {
                    trim_whitespace(course);
                    generate_class_report(&db, course);
                }
                break;
            }
            
            case 16: { // Advanced Search
                char search_term[100];
                printf("Enter search term (searches name, email, course, phone): ");
                if (fgets(search_term, sizeof(search_term), stdin)) {
                    trim_whitespace(search_term);
                    Student results[MAX_STUDENTS];
                    int count = advanced_search(&db, search_term, results, MAX_STUDENTS);
                    if (count > 0) {
                        printf("\nFound %d students matching '%s':\n", count, search_term);
                        sort_students_by_name(results, count);
                        for (int i = 0; i < count; i++) {
                            printf("%d. %s %s (ID: %d, Email: %s, Course: %s)\n", 
                                   i + 1, results[i].first_name, results[i].last_name, 
                                   results[i].id, results[i].email, results[i].course);
                        }
                    } else {
                        printf("No students found matching '%s'.\n", search_term);
                    }
                }
                break;
            }
            
            case 17: { // Save Database
                printf("Enter filename (or press Enter for default '%s'): ", filename);
                char input[MAX_FILENAME_LENGTH];
                if (fgets(input, MAX_FILENAME_LENGTH, stdin)) {
                    trim_whitespace(input);
                    if (strlen(input) > 0) {
                        strncpy(filename, input, MAX_FILENAME_LENGTH - 1);
                        filename[MAX_FILENAME_LENGTH - 1] = '\0';
                    }
                }
                save_database(&db, filename);
                break;
            }
            
            case 18: { // Load Database
                printf("Enter filename (or press Enter for default '%s'): ", filename);
                char input[MAX_FILENAME_LENGTH];
                if (fgets(input, MAX_FILENAME_LENGTH, stdin)) {
                    trim_whitespace(input);
                    if (strlen(input) > 0) {
                        strncpy(filename, input, MAX_FILENAME_LENGTH - 1);
                        filename[MAX_FILENAME_LENGTH - 1] = '\0';
                    }
                }
                load_database(&db, filename);
                break;
            }
            
            case 19: { // Exit
                printf("Save database before exiting? (y/n): ");
                char save_choice;
                if (scanf("%c", &save_choice) == 1 && (save_choice == 'y' || save_choice == 'Y')) {
                    save_database(&db, filename);
                }
                printf("Thank you for using Student Record System!\n");
                cleanup_database(&db);
                return 0;
            }
            
            default:
                printf("Invalid choice. Please select a number between 1 and 19.\n");
                break;
        }
        
        printf("\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}