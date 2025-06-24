#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 20
#define MAX_ADDRESS_LENGTH 200
#define MAX_COURSE_LENGTH 50
#define MAX_FILENAME_LENGTH 100
#define MAX_STUDENTS 1000
#define MAX_SUBJECTS 10

// Grade structure
typedef struct {
    char subject[MAX_COURSE_LENGTH];
    float grade;
    int credits;
} Grade;

// Student structure
typedef struct {
    int id;
    char first_name[MAX_NAME_LENGTH];
    char last_name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char address[MAX_ADDRESS_LENGTH];
    char course[MAX_COURSE_LENGTH];
    int enrollment_year;
    Grade grades[MAX_SUBJECTS];
    int num_grades;
    float gpa;
    int is_active;
} Student;

// Database structure
typedef struct {
    Student students[MAX_STUDENTS];
    int count;
    int next_id;
} StudentDatabase;

// Function prototypes

// Core database operations
int init_database(StudentDatabase *db);
int save_database(StudentDatabase *db, const char *filename);
int load_database(StudentDatabase *db, const char *filename);
void cleanup_database(StudentDatabase *db);

// Student operations
int add_student(StudentDatabase *db, Student *student);
int update_student(StudentDatabase *db, int id, Student *updated_student);
int delete_student(StudentDatabase *db, int id);
Student* find_student_by_id(StudentDatabase *db, int id);
Student* find_student_by_name(StudentDatabase *db, const char *first_name, const char *last_name);

// Grade operations
int add_grade(StudentDatabase *db, int student_id, const char *subject, float grade, int credits);
int update_grade(StudentDatabase *db, int student_id, const char *subject, float new_grade);
int delete_grade(StudentDatabase *db, int student_id, const char *subject);
float calculate_gpa(Student *student);
void update_student_gpa(Student *student);

// Search and filter operations
int search_students_by_course(StudentDatabase *db, const char *course, Student results[], int max_results);
int search_students_by_year(StudentDatabase *db, int year, Student results[], int max_results);
int get_students_by_gpa_range(StudentDatabase *db, float min_gpa, float max_gpa, Student results[], int max_results);

// Display and reporting
void display_student(const Student *student);
void display_all_students(const StudentDatabase *db);
void display_student_grades(const Student *student);
void generate_transcript(const Student *student);
void generate_class_report(StudentDatabase *db, const char *course);

// Utility functions
int validate_email(const char *email);
int validate_phone(const char *phone);
void trim_whitespace(char *str);
int get_current_year(void);

// Input/Output helpers
void clear_input_buffer(void);
int get_student_input(Student *student);
void print_menu(void);

// Sorting functions
void sort_students_by_name(Student students[], int count);
void sort_students_by_id(Student students[], int count);
void sort_students_by_gpa(Student students[], int count);

#endif // STUDENT_H