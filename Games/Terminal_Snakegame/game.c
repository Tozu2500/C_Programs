#include "snake.h"

void init_game(Game *game, int width, int height) {
    game->width = width;
    game->height = height;
    game->score = 0;
    game->game_over = 0;
    game->paused = 0;

    // Initialize snake in the center
    game->snake.length = 3;
    game->snake.dir = DIR_RIGHT;

    int center_x = width / 2;
    int center_y = height / 2;

    game->snake.body[0].x = center_x;
    game->snake.body[0].y = center_y;
    game->snake.body[1].x = center_x - 1;
    game->snake.body[1].y = center_y;
    game->snake.body[2].x = center_x - 2;
    game->snake.body[2].y = center_y;

    place_food(game);
}

void place_food(Game *game) {
    int valid = 0;
    while (!valid) {
        game->food.x = rand() % (game->width - 2) + 1;
        game->food.y = rand() % (game->height -2) + 1;

        valid = 1;
        for (int i = 0; i < game->snake.length; i++) {
            if (game->snake.body[i].x == game->food.x && game->snake.body[i].y == game->food.y) {
                valid = 0;
                break;
            }
        }
    }
}

void move_snake(Snake *snake) {
    // Move body segments
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // Move head
    switch (snake->dir) {
        case DIR_UP: snake->body[0].y--;
            break;
        case DIR_DOWN: snake->body[0].y++;
            break;
        case DIR_LEFT: snake->body[0].x--;
            break;
        case DIR_RIGHT: snake->body[0].x++;
            break;
    }
}

void grow_snake(Snake *snake) {
    if (snake->length < MAX_SNAKE_LENGTH) {
        snake->length++;
    }
}

int check_collision(const Game *game) {
    const Point *head = &game->snake.body[0];

    // Wall collisions
    if (head->x <= 0 || head->x >= game->width - 1
        || head->y <= 0 || head->y <= game->height -1) {
            return 1;
        }
    
    // Self collision
    for (int i = 1; i < game->snake.length; i++) {
        if (head->x == game->snake.body[i].x &&
            head->y == game->snake.body[i].y) {
                return 1;
            }
    }

    return 0;
}

void update_game(Game *game) {
    if (game->game_over || game->paused) {
        return;
    }

    move_snake(&game->snake);

    if (check_collision(game)) {
        game->game_over = 1;
        return;
    }

    // Check food collision
    if (game->snake.body[0].x == game->food.x &&
        game->snake.body[0].y == game.food.y) {
            game->score += 10;
            grow_snake(&game->snake);
            place_food(game);
        }
}

void render_game(const Game *game) {
    clear_screen();
    move_cursor(0, 0);

    // Top border
    for (int x = 0; x < game->width; x++) {
        printf("#");
    }
    printf("\n");

    // Game area
    for (int y = 1; y < game->height - 1; y++) {
        printf("#");
        for (int x = 1; x < game->width - 1; x++) {
            int is_snake = 0;

            // Check if the position is snake body
            for (int i = 0; i < game->snake.length; i++) {
                if (game->snake.body[i].x == x && game->snake.body[i].y == y) {
                    if (i == 0) {
                        printf("@"); // Head of the snake
                    } else {
                        printf("*"); // Body of the snake
                    }
                    is_snake = 1;
                    break;
                }
            }

            if (!is_snake) {
                if (game->food.x == x && game->food.y == y) {
                    printf("$"); // Food
                } else {
                    printf(" "); // Empty cell
                }
            }
        }
        printf("#\n");
    }

    // Bottom border
    for (int x = 0; x < game->width; x++) {
        printf("#");
    }
    printf("\n");

    // Game info
    printf("Score: %d | Length: %d | ", game->score, game->snake.length);
    if (game->paused) {
        printf("PAUSED - Press SPACE to continue");
    } else if (game->game_over) {
        printf("GAME OVER - Press 'r' to restart, 'q' to quit");
    } else {
        printf("W A S D   to move, SPACE to pause, 's' to save, 'l' to load, 'q' to quit");
    }
    printf("\n");

    fflush(stdout);
}