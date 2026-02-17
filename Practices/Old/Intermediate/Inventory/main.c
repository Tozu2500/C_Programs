#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define INITIAL_CAPACITY 16
#define MAX_LINE 1024
#define MAX_NAME 128
#define MAX_ID   64

typedef struct {
    char id[MAX_ID];
    char name[MAX_NAME];
    double price;
    int qty;
} Item;

typedef struct {
    Item* data;
    size_t size;
    size_t capacity;
    int sorted; // 1 if sorted ascending by id
} Inventory;

// ---------- Utility ----------

static void die(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

static void* xmalloc(size_t n) {
    void* p = malloc(n);
    if (!p) die("Out of memory");
    return p;
}

static void* xrealloc(void* p, size_t n) {
    void* r = realloc(p, n);
    if (!r) die("Out of memory (realloc)");
    return r;
}

static int valid_id(const char* s) {
    if (!s || !*s) return 0;
    for (const char* c = s; *c; ++c) {
        if (!(isalnum((unsigned char)*c) || *c == '_' || *c == '-')) return 0;
    }
    return 1;
}

static int valid_name(const char* s) {
    if (!s || !*s) return 0;
    for (const char* c = s; *c; ++c) {
        if (*c == ',') return 0;
    }
    return 1;
}

static int split_csv_line(char* line, char** id, char** name, char** price, char** qty) {
    // Mutates line by inserting NULs; expects 4 fields.
    // Returns 1 on success, 0 otherwise.
    int fields = 0;
    char* p = line;
    *id = *name = *price = *qty = NULL;

    // Trim newline
    line[strcspn(line, "\r\n")] = '\0';

    // Find commas
    char* parts[4] = {0};
    while (fields < 4) {
        parts[fields] = p;
        char* comma = strchr(p, ',');
        if (!comma) {
            if (fields == 3) break; // last field
            return 0;
        }
        *comma = '\0';
        p = comma + 1;
        fields++;
    }
    if (fields != 3) return 0;

    *id = parts[0];
    *name = parts[1];
    *price = parts[2];
    *qty = p;
    return 1;
}