#include "snake.h"

void show_menu(Stats *stats) {
    clear_screen();
    move_cursor(0, 0);

    printf("Terminal Snake Game\n");
    printf("1. New game\n");
    printf("2. Load game\n");
    printf("3. View statistics\n");
    printf("4. Exit\n");
    printf("Choose option (1-4): ");
    fflush(stdout);
}

void show_stats(const Stats *stats) {
    clear_screen();
    move_cursor(0, 0);

    printf("Overall Statistics:\n");
    printf("Games played: %d\n", stats->games_played);
    printf("Highest score: %d\n", stats->high_score);

    if (stats->games_played > 0) {
        printf("Average score: %.1f\n", (float)stats->total_score / stats->games_played);
        printf("Total food eaten: %d\n", stats->total_food_eaten);

        if (stats->last_played > 0) {
            printf("Last played: %s", ctime(&stats->last_played));
        } else {
            printf("Last played: Never\n");
        }
    }

    printf("\nPress any key to return to the menu...\n");
    fflush(stdout);
}

void game_loop(Game *game, Stats *stats) {
    clock_t last_update = clock();
    const clock_t update_interval = CLOCKS_PER_SEC / 10; // 10 FPS

    render_game(game);

    while (!game->game_over) {
        if (kbhit()) {
            char key = getch();

            switch (key) {
                case 'w': case 'W':
                    if (game->snake.dir != DIR_DOWN && !game->paused) {
                        game->snake.dir = DIR_UP;
                    }
                    break;
                case 's': case 'S':
                    if (!game->paused && game->snake.dir != DIR_UP) {
                        game->snake.dir = DIR_DOWN;
                    } else if (game->paused || game->game_over) {
                        // Save
                        if (save_game(game)) {
                            move_cursor(0, game->height + 2);
                            printf("Game saved successfully!    \n");
                            fflush(stdout);
                            usleep(500000);
                        }
                    }
                    break;
                case 'a': case 'A':
                    if (game->snake.dir != DIR_RIGHT && !game->paused) {
                        game->snake.dir = DIR_LEFT;
                    }
                    break;
                case 'd': case 'D':
                    if (game->snake.dir != DIR_LEFT && !game->paused) {
                        game->snake.dir = DIR_RIGHT;
                    }
                    break;
                case ' ':
                    game->paused = !game->paused;
                    break;
                case 'l': 'L':
                    if (game->paused || game->game_over) {
                        Game temp_game;
                        if (load_game(&temp_game)) {
                            *game = temp_game;
                            move_cursor(0, game->height + 2);
                            printf("Game loaded successfully    \n");
                            fflush(stdout);
                            usleep(500000);
                        }
                    }
                    break;
                case 'r': case 'R':
                    if (game->game_over) {
                        update_stats(stats, game);
                        init_game(game, game->width, game->height);
                    }
                    break;
                case 'q': case 'Q':
                    if (game->game_over) {
                        update_stats(stats, game);
                    }
                    return;
            }
        }

        clock_t now = clock();
        if (now - last_update >= update_interval) {
            update_game(game);
            render_game(game);
            last_update = now;
        }

        usleep(10000);  // Small delay to prevent high CPU usage
    }

    // Game over - input wait
    while (1) {
        if (kbhit()) {
            char key = getch();
            switch (key) {
                case 'r': case 'R':
                    update_stats(stats, game);
                    init_game(game, game->width, game->height);
                    game_loop(game, stats);
                    return;
                case 's': case 'S':
                    if (save_game(game)) {
                        move_cursor(0, game->height + 2);
                        printf("Game saved successfully!    \n");
                        fflush(stdout);
                        usleep(500000);
                        render_game(game);
                    }
                    break;
                case 'l': case 'L': {
                    Game temp_game;
                    if (load_game(&temp_game)) {
                        *game = temp_game;
                        move_cursor(0, game->height + 2);
                        printf("Game loaded successfully!    \n");
                        fflush(stdout);
                        usleep(500000);
                        game_loop(game, stats);
                        return;
                    }
                    break;
                }
                case 'q': case 'Q':
                    update_stats(stats, game);
                    return;
            }
        }
        usleep(10000);
    }
}

int main(void) {
    srand(time(NULL));

    Stats stats = load_stats();
    Game game;

    setup_terminal();

    while (1) {
        show_menu(&stats);

        char choice = 0;
        while (choice < '1' || choice > '4') {
            if (kbhit()) {
                choice = getch();
            }
            usleep(10000);
        }

        switch (choice) {
            case '1':
                init_game(&game, 40, 20);
                game_loop(&game, &stats);
                break;

            case '2': {
                if (load_game(&game)) {
                    game_loop(&game, &stats);
                } else {
                    clear_screen();
                    move_cursor(0, 0);
                    printf("No saved game found. Press any key to continue...");
                    fflush(stdout);
                    while (!kbhit()) usleep(10000);
                    getch();
                }
                break;
            }

            case '3':
                show_stats(&stats);
                while (!kbhit()) usleep(10000);
                getch();
                break;

            case '4':
                restore_terminal();
                printf("Thanks for playing!\n");
                return 0;
        }
    }

    restore_terminal();
    return 0;
}