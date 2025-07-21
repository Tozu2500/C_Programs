#include "snake.h"

static struct termios orig_termios;

void setup_terminal(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    hide_cursor();
}

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    show_cursor();
    clear_screen();
    move_cursor(0, 0);
}

int kbhit(void) {
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1; // Key was pressed
    }
    return 0; // No key pressed
}

char getch(void) {
    int ch = getchar();
    return ch == EOF ? 0 : ch;
}

void clear_screen(void) {
    printf("\033[2J"); // ANSI escape code to clear screen
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape code to move cursor
}

void hide_cursor(void) {
    printf("\033[?25l"); // ANSI escape code to hide cursor
}

void show_cursor(void) {
    printf("\033[?25h"); // ANSI escape code to show cursor
}