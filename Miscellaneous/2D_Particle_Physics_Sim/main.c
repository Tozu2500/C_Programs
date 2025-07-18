#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif
#include "physics.h"

char screen_buffer[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
PhysicsSystem* physics_system;

void init_console(void);
void cleanup_console(void);
void clear_screen(void);
void render_frame(void);
void update_screen_buffer(void);
void draw_particle_to_buffer(Particle* particle);
int kbhit(void);
void setup_particles(void);
void handle_input(void);

#ifdef _WIN32
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hide_cursor(void) {
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
#else
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

void hide_cursor(void) {
    printf("\033[?25l");
}

int kbhit(void) {
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
#endif

int main(void) {
    srand(time(NULL));

    init_console();

    physics_init(&physics_system);
    setup_particles();

    printf("Physics Simulation - Press 'q' to quit, 'r' to reset and 'a' to add a particle!\n");
    printf("Particless will bounce around the screen\n");
    printf("Press any key to start...\n");
    getchar();

    clock_t last_time = clock();
    int frame_count = 0;

    while (1) {
        clock_t current_time = clock();
        float delta_time = (float)(current_time - last_time) / CLOCKS_PER_SEC;
        last_time = current_time;

        // Delta time cap, preventing huge jumps
        if (delta_time > 0.033f) delta_time = 0.033f;

        handle_input();

        physics_update(&physics_system, delta_time);

        // Render every few frames for flickering reducing
        if (frame_count % 2 == 0) {
            render_frame();
        }
        frame_count++;

        #ifdef _WIN32
            Sleep(16);
        #else
            usleep(16000);
        #endif
    }

    cleanup_console();
    return 0;
}

void init_console(void) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #else
        struct termios term;
        tcgetattr(0, &term);
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &term);
        fcntl(0, F_SETFL, O_NONBLOCK);
    #endif

    hide_cursor();
    clear_screen();
}

void cleanup_console(void) {
    #ifndef _WIN32
        struct termios term;
        tcgetattr(0, &term);
        term.c_lflag |= (ICANON | ECHO);
        tcsetattr(0, TCSANOW, &term);
        printf("\033[?25h");
    #endif

    clear_screen();
    printf("Physics simulation ended.\n");
}

void clear_screen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void render_frame(void) {
    // Clear the buffer
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen_buffer[y][x] = ' ';
        }
        screen_buffer[y][SCREEN_WIDTH] = '\0';
    }

    // Border drawing
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        screen_buffer[0][x] = '-';
        screen_buffer[SCREEN_HEIGHT - 1][x] = '-';
    }
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        screen_buffer[y][0] = '|';
        screen_buffer[y][SCREEN_WIDTH - 1] = '|';
    }

    // Particle drawing
    for (int i = 0; i < physics_system->particle_count; i++) {
        if (physics_system.particles[i].active) {
            draw_particle_to_buffer(&physics_system.particles[i]);
        }
    }

    // Render to screen
    gotoxy(0, 0);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        printf("%s\n", screen_buffer[y]);
    }

    // Show info
    printf("Particles: %d/%d | Controls: q=quit, r=reset, a=add particle\n",
            physics_system->particle_count, MAX_PARTICLES);
    
    fflush(stdout);
}

void draw_particle_to_buffer(Particle* particle) {
    int x = (int)(particle->position.x);
    int y = (int)(particle->position.y);

    // Keep in with the bounds
    if (x >= 1 && x < SCREEN_WIDTH - 1 && y >= 1 && y < SCREEN_HEIGHT - 1) {
        // Using different characters based on the particle index for variety
        char particle_chars[] = {'*', 'O', 'o', '+', '#', '@', '%'};
        int char_index = ((int)particle->position.x + (int)particle->position.y) % 8;
        screen_buffer[y][x] = particle_chars[char_index];
    }
}

void setup_particles(void) {
    // Adding some initial particles with different starting positions and velocities.
    physics_add_particle(&physics_system, 10, 5, 8, 2);
    physics_add_particle(&physics_system, 20, 8, -5, 3);
    physics_add_particle(&physics_system, 30, 10, 3, -2);
    physics_add_particle(&physics_system, 40, 6, -7, 4);
    physics_add_particle(&physics_system, 50, 12, 6, -3);
    physics_add_particle(&physics_system, 60, 9, -4, 5);
}

void handle_input(void) {
    #ifdef _WIN32
        if (_kbhit()) {
            char key = _getch();
    #else
        char key;
        if (read(0, &key, 1) > 0) {
    #endif
        switch (key) {
            case 'q':
            case 'Q':
                cleanup_console();
                exit(0);
                break;
            case 'r':
            case 'R':
                // Reset simulation
                physics_init(&physics_system);
                setup_particles();
                break;
            case 'a':
            case 'A':
                // Add a random particle
                if (physics_system.particle_count < MAX_PARTICLES) {
                    float x = 5 + rand() % (SCREEN_WIDTH - 10);
                    float y = 3 + rand() % (SCREEN_HEIGHT - 6);
                    float vx = (rand() % 20 - 10) * 0.5f;
                    float vy = (rand() % 20 - 10) * 0.5f;
                    physics_add_particle(&physics_system, x, y, vx, vy);
                }
                break;
        }
    }
#ifndef _WIN32
    }
#endif
}
