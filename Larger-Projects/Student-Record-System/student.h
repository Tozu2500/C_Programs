#ifndef STUDENT_H
#define STUDENT_H

#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100
#define FILENAME "students.dat"

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    float gpa;
    char course[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
    char phone[20];
    int enrollmentYear;
} Student;

// Function protos
void addStudent(Student students[], int *count);
void displayAllStudents(Student students[], int count);
void searchStudent(Student students[], int count);
void updateStudent(Student students[], int count);
void deleteStudent(Student students[], int *count);
void displayStudentInfo(Student student);

#endif // STUDENT_H