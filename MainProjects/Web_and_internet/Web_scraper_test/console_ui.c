#include "console_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>

console_t* console_init(void) {
    console_t* console = malloc(sizeof(console_t));
    if (!console) return NULL;

    console->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console->hConsole == INVALID_HANDLE_VALUE) {
        free(console);
        return NULL;
    }

    GetConsoleScreenBufferInfo(console->hConsole, &console->csbi);
    console->screen_size.X = console->csbi.srWindow.Right - console->csbi.srWindow.Left + 1;
    console->screen_size.Y = console->csbi.srWindow.Bottom - console->csbi.srWindow.Top + 1;
    console->current_line = 0;

    SetConsoleTitle("Web scraper 1.0");

    return console;
}

void console_cleanup(console_t* console) {
    if (console) {
        console_reset_color(console);
        free(console);
    }
}

void console_clear(console_t* console) {
    if (!console) return;

    COORD coord = {0, 0};
    DWORD written;
    DWORD size = console->screen_size.X * console->screen_size.Y;

    FillConsoleOutputCharacterA(console->hConsole, ' ', size, coord, &written);
    FillConsoleOutputPosition(console->hConsole, console->csbi.wAttributes, size, coord &written);
    SetConsoleCursorPosition(console->hConsole, coord);

    console->currrent_line = 0;
}

void console_set_color(console_t* console, console_color_t fg, console_color_t bg) {
    if (!console) return;

    WORD attributes = (bg << 4) | fg;
    SetConsoleTextAttribute(console->hConsole, attributes);
}

void console_reset_color(console_t* console) {
    if (!console) return;

    SetConsoleTextAttribute(console->hConsole, console->csbi.wAttributes);
}

void console_goto_xy(console_t* console, int x, int y) {
    if (!console) return;

    COORD coord = {x, y};
    SetConsoleCursorPosition(console->hConsole, coord);
}

void console_print(console_t* console, const char* format, ...) {
    if (!console || !format) return;

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    console->current_line++;
}

void console_print_colored(console_t* console, console_color_t fg, console_color_t bg, const char* format, ...) {
    if (!console || !format) return;

    console_set_color(console, fg, bg);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    console_reset_color(console);
    console->current_line++;
}

void console_print_header(console_t* console, const char* title) {
    if (!console || !title) return;

    int title_len = strlen(title);
    int padding = (console->screen_size.X - title_len - 4) / 2;

    console_set_color(console, COLOR_WHITE, COLOR_BLUE);

    for (int i = 0; i < console->screen_size.X; i++) {
        printf(" ");
    }
    printf("\n");

    for (int i = 0; i < padding; i++) printf(" ");
    printf("  %s  ", title);
    for (int i = 0; i < console->screen_size.X - padding - title_len - 4; i++) printf(" ");
    printf("\n");

    for (int i = 0; i < console->screen_size.X; i++) {
        printf(" ");
    }
    printf("\n");

    console_reset_color(console);
    console->current_line += 3;
}

void console_print_separator(console_t* console) {
    if (!console) return;

    console_set_color(console, COLOR_DARK_GRAY, COLOR_BLACK);
    for (int i = 0; i < console->screen_size.X; i++) {
        printf("-");
    }
    printf("\n");
    console_reset_color(console);
    console->current_line++;
}

void console_print_status(console_t* console, const char* status, int success) {
    if (!console || !status) return;

    console_color_t color = success ? COLOR_GREEN : COLOR_RED;
    const char* prefix = success ? "[OK] " : "[ERROR] ";

    console_print_colored(console, color, COLOR_BLACK, "%s%s\n", prefix, status);
}

char* console_get_input(console_t* console, const char* prompt) {
    if (!console || !prompt) return NULL;

    console_set_color(console, COLOR_CYAN, COLOR_BLACK);
    printf("%s", prompt);
    console_reset_color(console);

    char* input = malloc(MAX_INPUT_LENGTH);
    if (!input) return NULL;

    if (fgets(input, MAX_INPUT_LENGTH, stdin)) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        console->current_line++;
        return input;
    }

    free(input);
    return NULL;
}

char* console_get_password(console_t* console, const char* prompt) {
    if (!console || !prompt) return NULL;

    console_set_color(console, COLOR_CYAN, COLOR_BLACK);
    printf("%s", prompt);
    console_reset_color(console);

    char* password = malloc(MAX_INPUT_LENGTH);
    if (!password) return NULL;

    int i = 0;
    int ch;

    while ((ch = _getch()) != '\r' && i < MAX_INPUT_LENGTH - 1) {
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch >= 32 && ch <= 126) {
            printf("*");
            password[i++] = ch;
        }
    }

    password[i] = '\0';
    printf("\n");
    console->current_line++;

    return password;
}

int console_get_yes_no(console_t* console, const char* prompt) {
    if (!console || !prompt) return 0;

    while (1) {
        char* input = console_get_input(console, prompt);
        if (!input) continue;

        if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0 ||
            strcmp(input, "yes") == 0 || strcmp(input, "Yes") == 0) {
                free(input);
                return 1;
            } else if (strcmp(input, "n") == 0 || strcmp(input, "N") == 0 ||
                        strcmp(input, "no") == 0 || strcmp(input, "No") == 0) {
                            free(input);
                            return 0;
                        }

            free(input);
            console_print_colored(console, COLOR_RED, COLOR_BLACK, "Please enter 'y' or 'n'\n");
    }
}

int console_get_number(console_t* console, const char* prompt, int min, int max) {
    if (!console || !prompt) return min;

    while (1) {
        char* input = console_get_input(console, prompt);
        if (!input) continue;

        int number = atoi(input);
        free(input);

        if (number >= min && number <= max) {
            return number;
        }

        console_print_header(console, COLOR_RED, COLOR_BLACK, "Please enter a number between %d and %d\n", min, max);
    }
}

menu_t* menu_create(const char* title) {
    if (!title) return NULL;

    menu_t* menu = malloc(sizeof(menu_t));
    if (!menu) return NULL;

    strncpy(menu->title, title, sizeof(menu->title) - 1);
    menu->title[sizeof(menu->title) - 1] = '\0';
    menu->item_count = 0;
    menu->selected_index = 0;

    return menu;
}

void menu_free(menu_t* menu) {
    if (menu) {
        free(menu);
    }
}

void menu_add_item(menu_t* menu, const char* text, int value) {
    if (!menu || !text || menu->item_count >= MAX_MENU_ITEMS) return;

    menu_item_t* item = &menu->items[menu->item_count++];
    strncpy(item->text, text, sizeof(item->text) - 1);
    item->text[sizeof(item->text) - 1] = '\0';
    item->value = value;
}

int menu_show(console_t* console, menu_t* menu) {
    if (!console || !menu || menu->item_count == 0) return -1;

    while (1) {
        console_clear(console);
        console_print_header(console, menu->title);
        printf("\n");

        for (int i = 0; i < menu->item_count; i++) {
            if (i == menu->selected_index) {
                console_print_colored(console, COLOR_BLACK, COLOR_WHITE, " > %s\n", menu->items[i].text);
            } else {
                console_print(console, "  %s\n", menu->items[i].text);
            }
        }

        printf("\nUse arrow keys to navigate, Enter to select, ESC to exit\n");

        int key = _getch();

        if (key == 224) {
            key = _getch();
            switch (key) {
                case 72:
                    menu->selected_index = (menu->selected_index - 1 + menu->item_count) % menu->item_count;
                    break;
                case 80:
                    menu->selected_index = (menu->selected_index + 1) % menu->item_count;
                    break;
            }
        } else if (key == 13) {
            return menu->items[menu->selected_index].value;
        } else if (key == 27) {
            return -1;
        }
    }
}

void console_show_progress(console_t* console, int current, int total, const char* message) {
    if (!console || total <= 0) return;

    int progress_width = 50;
    int filled = (current * progress_width) / total;
    int percentage = (current * 100) / total;

    printf("\r%s [", message ? message : "Progress");

    console_set_color(console, COLOR_GREEN, COLOR_BLACK);
    for (int i = 0; i < filled; i++) {
        printf("=");
    }
    console_reset_color(console);

    for (int i = filled; i < progress_width; i++) {
        printf(" ");
    }

    printf("] %d%% (%d/%d)", percentage, current, total);
    fflush(stdout);

    if (current >= total) {
        printf("\n");
        console->current_line++;
    }
}

void console_wait_key(console_t* console, const char* message) {
    if (!console) return;

    console_print_colored(console, COLOR_YELLOW, COLOR_BLACK,
                "%s\n", message ? message : "Press any key to continue...");
    _getch();
}

void console_draw_box(console_t* console, int x, int y, int width, int height) {
    if (!console || width <= 0 || height <= 0) return;

    console_set_color(console, COLOR_WHITE, COLOR_BLACK);

    for (int i = 0; i < height; i++) {
        console_goto_xy(console, x, y + i);
        if (i == 0 || i == height - 1) {
            for (int j = 0; j < width; j++) {
                printf("-");
            }
        } else {
            printf("|");
            for (int j = 1; j < width - 1; j++) {
                printf(" ");
            }
            printf("|");
        }
    }

    console_reset_color(console);
}

void console_center_text(console_t* console, int y, const char* text) {
    if (!console || !text) return;

    int text_len = strlen(text);
    int x = (console->screen_size.X - text_len) / 2;

    console_goto_xy(console, x, y);
    printf("%s", text);
}