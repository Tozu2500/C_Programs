#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char *name;
    float gpa;
} Student;

typedef int (*CompareFn)(const Student*, const Student*);

void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

char *strdup_safe(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *d = malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

Student *create_student(int id, const char *name, float gpa) {
    Student *s = malloc(sizeof(Student));
    if (!s) return NULL;
    s->id = id;
    s->name = strdup_safe(name);
    s->gpa = gpa;
    return s;
}

void free_student(Student *s) {
    if (!s) return;
    free(s->name);
    free(s);
}

void print_student(const Student *s) {
    printf("ID: %d | Name: %s | GPA: %.2f\n", s->id, s->name ? s->name : "(null)", s->gpa);
}

typedef struct {
    Student *data;
    size_t size;
    size_t capacity;
} StudentDB;

void init_db(StudentDB *db) {
    db->data = NULL;
    db->size = 0;
    db->capacity = 0;
}

void free_db(StudentDB *db) {
    if (!db) return;
    for (size_t i = 0; i < db->size; ++i) {
        free(db->data[i].name);
    }
    free(db->data);
    db->data = NULL;
    db->size = db->capacity = 0;
}

int ensure_capacity(StudentDB *db, size_t mincap) {
    if (db->capacity >= mincap) return 1;
    size_t newcap = db->capacity ? db->capacity * 2 : 4;
    while (newcap < mincap) newcap *= 2;

    Student *tmp = realloc(db->data, newcap * sizeof(Student));
    if (!tmp) return 0;
    db->data = tmp;
    db->capacity = newcap;
    return 1;
}

int add_student_db(StudentDB *db, int id, const char *name, float gpa) {
    if (!ensure_capacity(db, db->size + 1)) return 0;
    db->data[db->size].id = id;
    db->data[db->size].name = strdup_safe(name);
    db->data[db->size].gpa = gpa;
    db->size++;
    return 1;
}

int find_student_index(const StudentDB *db, int id) {
    for (size_t i = 0; i < db->size; ++i) {
        if (db->data[i].id == id) return (int)i;
    }
    return -1;
}

int delete_student_db(StudentDB *db, int id) {
    int idx = find_student_index(db, id);
    if (idx < 0) return 0;
    free(db->data[idx].name);

    for (size_t i = idx; i + 1 < db->size; ++i) {
        db->data[i] = db->data[i + 1];
    }
    db->size--;
    return 1;
}

void list_students(const StudentDB *db) {
    if (db->size == 0) {
        puts("No students.");
        return;
    }
    for (size_t i = 0; i < db->size; ++i) {
        print_student(&db->data[i]);
    }
}

int cmp_by_id(const Student *a, const Student *b) {
    return a->id - b->id;
}

int cmp_by_gpa(const Student *a, const Student *b) {
    if (a->gpa < b->gpa) return -1;
    if (a->gpa > b->gpa) return 1;
    return 0;
}

int cmp_by_name(const Student *a, const Student *b) {
    return strcmp(a->name, b->name);
}

void sort_db(StudentDB *db, CompareFn cmp) {
    if (db->size < 2) return;
    for (size_t i = 0; i < db->size - 1; ++i) {
        for (size_t j = 0; j < db->size - i - 1; ++j) {
            if (cmp(&db->data[j], &db->data[j + 1]) > 0) {
                Student tmp = db->data[j];
                db->data[j] = db->data[j + 1];
                db->data[j + 1] = tmp;
            }
        }
    }
}

void load_sample(StudentDB *db) {
    add_student_db(db, 1001, "Alice Johnson", 3.6f);
    add_student_db(db, 1002, "Bob Smith", 3.2f);
    add_student_db(db, 1003, "Charlie Brown", 2.9f);
    add_student_db(db, 1004, "Diana King", 3.9f);
}

int main(void) {
    StudentDB db;
    init_db(&db);
    load_sample(&db);

    while (1) {
        printf("\nStudent DB: size=%zu capacity=%zu\n", db.size, db.capacity);
        puts("1) List  2) Add  3) Delete  4) Find  5) Sort  6) Quit");
        printf("Choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) { clear_input(); continue; }
        clear_input();
        if (choice == 1) {
            list_students(&db);
        } else if (choice == 2) {
            int id;
            char namebuf[128];
            float gpa;
            printf("Enter id: "); if (scanf("%d", &id) != 1) { clear_input(); continue; } clear_input();
            printf("Enter name: "); if (!fgets(namebuf, sizeof(namebuf), stdin)) continue;
            namebuf[strcspn(namebuf, "\n")] = '\0';
            printf("Enter gpa: "); if (scanf("%f", &gpa) != 1) { clear_input(); continue; } clear_input();
            if (add_student_db(&db, id, namebuf, gpa)) puts("Added.");
            else puts("Failed to add.");
        } else if (choice == 3) {
            int id;
            printf("Enter id to delete: "); if (scanf("%d", &id) != 1) { clear_input(); continue; } clear_input();
            if (delete_student_db(&db, id)) puts("Deleted.");
            else puts("Not found.");
        } else if (choice == 4) {
            int id;
            printf("Enter id to find: "); if (scanf("%d", &id) != 1) { clear_input(); continue; } clear_input();
            int idx = find_student_index(&db, id);
            if (idx >= 0) print_student(&db.data[idx]);
            else puts("Not found.");
        } else if (choice == 5) {
            puts("Sort by: 1) ID 2) Name 3) GPA");
            int s; if (scanf("%d", &s) != 1) { clear_input(); continue; } clear_input();
            if (s == 1) sort_db(&db, cmp_by_id);
            else if (s == 2) sort_db(&db, cmp_by_name);
            else sort_db(&db, cmp_by_gpa);
            puts("Sorted.");
        } else if (choice == 6) {
            break;
        } else {
            puts("Invalid.");
        }
    }
    
    free_db(&db);
    return 0;
}