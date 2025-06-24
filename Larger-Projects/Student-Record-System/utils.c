#include "student.h"
#include <ctype.h>

int validate_email(const char *email) {
    if (!email || strlen(email) < 5) {
        return 0;
    }

    // Basic level email validation
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

    return (at_count == 1 && dot_after_at && at_position > 0 && at_position < (int)strlen(email) - 2);
}

int validate_phone(const char *phone) {
    if (!phone) {
        return 0;
    }

    int digit_count = 0;

    for (int i = 0; phone[i]; i++) {
        if (isdigit(phone[i])) {
            digit_count++;
        } else if (phone[i] != ' ' && phone[i] != '-' && phone[i] != '(' && phone[i] != ')' && phone[i] != '+') {
            return 0; // Invalid character or a missing character was found
        }
    }

    return digit_count >= 10 && digit_count <= 15;
}

void trim_whitespace(char *str) {
    if (!str) return;

    // Trim leading whitespace
    int start = 0; // (index)
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

    printf("Enter student information: \n");

    printf("First name: ");
    if (!fgets(student->first_name, MAX_NAME_LENGTH, stdin)) return 0;
    trim_whitespace(student->first_name);
    if (strlen(student->first_name) == 0) {
        printf("Error: First name cannot be empty\n");
        return 0;
    }

    printf("Last name: ");
    if (!fgets(student->last_name, MAX_NAME_LENGTH, stdin)) return 0;
    trim_whitespace(student->last_name);
    if (strlen(student->last_name) == 0) {
        printf("Error: Last name cannot be empty\n");
        return 0;
    }

    printf("Email: ");
}