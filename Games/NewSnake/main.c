#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Game constants
#define WIDTH 40
#define HEIGHT 20
#define MAX_SNAKE_LENGTH 800

// Direction enumeration
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Point structure for coordinates
typedef struct {
    int x;
    int y;
} Point;

// Snake structure
typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    Direction direction;
} Snake;

// Game state structure
typedef struct {
    Snake snake;
    Point food;
    int score;
    int gameOver;
    int speed;
} GameState;

// Function prototypes
void initGame(GameState *game);
void drawGame(GameState *game);
void updateGame(GameState *game);
void handleInput(GameState *game);
void generateFood(GameState *game);
int checkCollision(GameState *game);
int checkSelfCollision(GameState *game);
void setCursorPosition(int x, int y);
void hideCursor();
void showGameOver(GameState *game);
void drawBorder();

// Main function
int main() {
    GameState game;
    
    // Hide cursor for better visuals
    hideCursor();
    
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize game
    initGame(&game);
    
    // Main game loop
    while (!game.gameOver) {
        drawGame(&game);
        handleInput(&game);
        updateGame(&game);
        
        // Dynamic speed based on score
        Sleep(game.speed);
    }
    
    // Show game over screen
    showGameOver(&game);
    
    return 0;
}

// Initialize game state
void initGame(GameState *game) {
    // Initialize snake in the middle
    game->snake.length = 3;
    game->snake.direction = RIGHT;
    
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    
    for (int i = 0; i < game->snake.length; i++) {
        game->snake.body[i].x = startX - i;
        game->snake.body[i].y = startY;
    }
    
    // Initialize game variables
    game->score = 0;
    game->gameOver = 0;
    game->speed = 100;
    
    // Generate first food
    generateFood(game);
    
    // Clear screen
    system("cls");
}

// Draw the game
void drawGame(GameState *game) {
    // Move cursor to top
    setCursorPosition(0, 0);
    
    // Draw top border
    printf("\n  ");
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf("\n");
    
    // Draw game area
    for (int y = 0; y < HEIGHT; y++) {
        printf("  #");
        for (int x = 0; x < WIDTH; x++) {
            int isSnake = 0;
            int isHead = 0;
            
            // Check if this position is part of the snake
            for (int i = 0; i < game->snake.length; i++) {
                if (game->snake.body[i].x == x && game->snake.body[i].y == y) {
                    isSnake = 1;
                    if (i == 0) {
                        isHead = 1;
                    }
                    break;
                }
            }
            
            // Check if this position is food
            if (game->food.x == x && game->food.y == y) {
                printf("@");
            } else if (isHead) {
                printf("O");
            } else if (isSnake) {
                printf("o");
            } else {
                printf(" ");
            }
        }
        printf("#\n");
    }
    
    // Draw bottom border
    printf("  ");
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf("\n");
    
    // Display score and instructions
    printf("\n  Score: %d", game->score);
    printf("\n  Speed: %d", 150 - game->speed);
    printf("\n\n  Controls: Arrow Keys / WASD");
    printf("\n  Press 'Q' to quit");
    printf("\n  Press 'P' to pause");
}

// Update game state
void updateGame(GameState *game) {
    // Store old tail position
    Point oldTail = game->snake.body[game->snake.length - 1];
    
    // Move body segments
    for (int i = game->snake.length - 1; i > 0; i--) {
        game->snake.body[i] = game->snake.body[i - 1];
    }
    
    // Move head based on direction
    switch (game->snake.direction) {
        case UP:
            game->snake.body[0].y--;
            break;
        case DOWN:
            game->snake.body[0].y++;
            break;
        case LEFT:
            game->snake.body[0].x--;
            break;
        case RIGHT:
            game->snake.body[0].x++;
            break;
    }
    
    // Check for wall collision
    if (checkCollision(game)) {
        game->gameOver = 1;
        return;
    }
    
    // Check for self collision
    if (checkSelfCollision(game)) {
        game->gameOver = 1;
        return;
    }
    
    // Check if snake ate food
    if (game->snake.body[0].x == game->food.x && 
        game->snake.body[0].y == game->food.y) {
        
        // Increase length
        game->snake.body[game->snake.length] = oldTail;
        game->snake.length++;
        
        // Increase score
        game->score += 10;
        
        // Increase speed (make game harder)
        if (game->speed > 50) {
            game->speed -= 2;
        }
        
        // Generate new food
        generateFood(game);
    }
}

// Handle user input
void handleInput(GameState *game) {
    if (_kbhit()) {
        char key = _getch();
        
        // Handle special keys (arrow keys)
        if (key == 0 || key == -32) {
            key = _getch();
            switch (key) {
                case 72: // Up arrow
                    if (game->snake.direction != DOWN) {
                        game->snake.direction = UP;
                    }
                    break;
                case 80: // Down arrow
                    if (game->snake.direction != UP) {
                        game->snake.direction = DOWN;
                    }
                    break;
                case 75: // Left arrow
                    if (game->snake.direction != RIGHT) {
                        game->snake.direction = LEFT;
                    }
                    break;
                case 77: // Right arrow
                    if (game->snake.direction != LEFT) {
                        game->snake.direction = RIGHT;
                    }
                    break;
            }
        } else {
            // Handle regular keys (WASD and quit)
            switch (key) {
                case 'w':
                case 'W':
                    if (game->snake.direction != DOWN) {
                        game->snake.direction = UP;
                    }
                    break;
                case 's':
                case 'S':
                    if (game->snake.direction != UP) {
                        game->snake.direction = DOWN;
                    }
                    break;
                case 'a':
                case 'A':
                    if (game->snake.direction != RIGHT) {
                        game->snake.direction = LEFT;
                    }
                    break;
                case 'd':
                case 'D':
                    if (game->snake.direction != LEFT) {
                        game->snake.direction = RIGHT;
                    }
                    break;
                case 'q':
                case 'Q':
                    game->gameOver = 1;
                    break;
                case 'p':
                case 'P':
                    // Pause game
                    printf("\n\n  PAUSED - Press any key to continue...");
                    _getch();
                    break;
            }
        }
    }
}

// Generate food at random position
void generateFood(GameState *game) {
    int validPosition = 0;
    
    while (!validPosition) {
        game->food.x = rand() % WIDTH;
        game->food.y = rand() % HEIGHT;
        
        validPosition = 1;
        
        // Check if food spawned on snake
        for (int i = 0; i < game->snake.length; i++) {
            if (game->snake.body[i].x == game->food.x && 
                game->snake.body[i].y == game->food.y) {
                validPosition = 0;
                break;
            }
        }
    }
}

// Check collision with walls
int checkCollision(GameState *game) {
    Point head = game->snake.body[0];
    
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
        return 1;
    }
    
    return 0;
}

// Check collision with self
int checkSelfCollision(GameState *game) {
    Point head = game->snake.body[0];
    
    for (int i = 1; i < game->snake.length; i++) {
        if (head.x == game->snake.body[i].x && head.y == game->snake.body[i].y) {
            return 1;
        }
    }
    
    return 0;
}

// Set cursor position in console
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Hide console cursor
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Show game over screen
void showGameOver(GameState *game) {
    system("cls");
    
    printf("\n\n");
    printf("  ╔════════════════════════════════════════╗\n");
    printf("  ║                                        ║\n");
    printf("  ║            GAME OVER!                  ║\n");
    printf("  ║                                        ║\n");
    printf("  ║         Final Score: %-6d           ║\n", game->score);
    printf("  ║         Snake Length: %-4d            ║\n", game->snake.length);
    printf("  ║                                        ║\n");
    printf("  ║     Thanks for playing Snake!          ║\n");
    printf("  ║                                        ║\n");
    printf("  ╚════════════════════════════════════════╝\n");
    
    printf("\n  Press any key to exit...");
    _getch();
}