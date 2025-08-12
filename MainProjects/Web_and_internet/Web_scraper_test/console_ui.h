#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <windows.h>

#define MAX_INPUT_LENGTH 512
#define MAX_MENU_ITEMS 16
#define MAX_OUTPUT_LINES 100

typedef enum {
    COLOR_BLACK = 0,
    COLOR_DARK_BLUE = 1,
    COLOR_DARK_GREEN = 2,
    COLOR_DARK_CYAN = 3,
    COLOR_DARK_RED = 4,
    COLOR_DARK_MAGENTA = 5,
    COLOR_DARK_YELLOW = 6,
    COLOR_LIGHT_GRAY = 7,
    COLOR_DARK_GRAY = 8,
    COLOR_BLUE = 9,
    COLOR_GREEN = 10,
    COLOR_CYAN = 11,
    COLOR_RED = 12,
    COLOR_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
} console_color_t;

typedef struct {
    char text[256];
    int value;
} menu_item_t;

typedef struct {
    char title[128];
    menu_item_t items[MAX_MENU_ITEMS];
    int item_count;
    int selected_index;
} menu_t;

typedef struct {
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD screen_size;
    int current_line;
} console_t;

console_t* console_init(void);
void console_cleanup(console_t* console);
void console_clear(console_t* console);
void console_set_color(console_t* console, console_color_t fg, console_color_t bg);
void console_reset_color(console_t* console);
void console_goto_xy(console_t* console, int x, int y);
void console_print(console_t* console, const char* format, ...);
void console_print_colored(console_t* console, console_color_t fg, console_color_t bg, const char* format, ...);
void console_print_header(console_t* console, const char* title);
void console_print_separator(console_t* console);
void console_print_status(console_t* console, const char* status, int success);

char* console_get_input(console_t* console, const char* prompt);
char* console_get_password(console_t* console, const char* prompt);
int console_get_yes_no(console_t* console, const char* prompt);
int console_get_number(console_t* console, const char* prompt, int min, int max);

menu_t* menu_create(const char* title);
void menu_free(menu_t* menu);
void menu_add_item(menu_t* menu, const char* text, int value);
int menu_show(console_t* console, menu_t* menu);

void console_show_progress(console_t* console, int current, int total, const char* message);
void console_wait_key(console_t* console, const char* message);

void console_draw_box(console_t* console, int x, int y, int width, int height);
void console_center_text(console_t* console, int y, const char* text);

#endif // CONSOLE_UI_H