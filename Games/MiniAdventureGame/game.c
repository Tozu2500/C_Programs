#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct for encapsulating player info
typedef struct {
    char name[50]; // Player's chosen name
    int health; // Health on a scale from 0 to 100
    int score; // Points for achievements
    int current_room; // Index of the current location in the array of rooms
    char inventory[5][30]; // Fixed size inventory with items
    int item_count; // Active inventory slots
} Player;

// Struct room structure
typedef struct {
    char name[50]; // Room name
    char description[200]; // Room desc
    int exits[4]; // North, south, east, west
    char items[3][30]; //
    int item_count; // Number of items in the room
} Room;

Player player; // instances
Room rooms[6];
int game_running = 1; // Boolean flag, game loop control

// Function prototypes
void initialize_game(void);
void display_room(void);
void process_command(char *command);
void move_player(int direction);
void take_item(char *item_name);
void show_inventory(void);
void show_help(void);
void game_over(void);

// Program entry point and game loop controller.
// Implements the game cycle: initialize, input processing and cleanup
int main(void) {

    char input[100]; // Buffer

    // Random number generation
    srand((unsigned int)time(NULL));

    printf("--- Castle Adventure ---\n");
    printf("Welcome to an interactive text game, you will navigate rooms\n");
    printf("collect items and explore the castle!\n\n");
    printf("Type 'help' anytime for commands.\n");
    
    // Game starting
    initialize_game();

    // Game loop
    while (game_running) {
        display_room();
        printf("\n>"); // Indicate command input

        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove newline char
            input[strcspn(input, "\n")] = 0;

            process_command(input);
        }
    }

    return 0;
}

// Game initialization

void initialize_game(void) {
    // Initialize player
    strcpy(player.name, "Explorer");
    player.health = 100;
    player.score = 0;
    player.current_room = 0;
    player.item_count = 0;

    // Initialize all exits to -1 to be blocked at first
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            rooms[i].exits[j] = -1;
        }
        rooms[i].item_count = 0;
    }

    // Room 0 (First)
    strcpy(rooms[0].name, "Castle Entrance");
    strcpy(rooms[0].description, "A grand stone entrance with wooden doors, torches flickering on the walls.");
    rooms[0].exits[0] = 1; // North to great hall

    // Room 1 - Great Hall
    strcpy(rooms[1].name, "Great Hall");
    strcpy(rooms[1].description, "A vast hall with a high ceiling, ancient paintings on the walls. Must be a main central.");
    rooms[1].exits[0] = 2; // North to library
    rooms[1].exits[1] = 0; // South to entrance
    rooms[1].exits[2] = 3; // East to kitchen
    rooms[1].exits[3] = 4; // West to armory
    strcpy(rooms[1].items[0], "painting");
    strcpy(rooms[1].items[1], "coin");
    rooms[1].item_count = 2;

    // Room 2 - Library
    strcpy(rooms[2].name, "Library");
    strcpy(rooms[2].description, "Countless books fill towering shelves and a mysterious glow is coming in from a corner.");
    rooms[2].exits[0] = -1; // North blocked
    rooms[2].exits[1] = 1; // South to Great Hall
    rooms[2].exits[2] = -1; // East blocked
    rooms[2].exits[3] = 5; // West to secret room
    strcpy(rooms[2].items[0], "book");
    strcpy(rooms[2].items[1], "scroll");
    rooms[2].item_count = 2;

    // Room 3 - Kitchen
    strcpy(rooms[3].name, "Kitchen");
    strcpy(rooms[3].description, "A large kitchen with a stone hearth, copper pots hanging from the ceiling.");
    rooms[3].exits[0] = -1; // North blocked
    rooms[3].exits[1] = -1; // South blocked
    rooms[3].exits[2] = -1; // East blocked
    rooms[3].exits[3] = 1; // West to great hall
    strcpy(rooms[3].items[0], "bread");
    strcpy(rooms[3].items[1], "knife");
    rooms[3].item_count = 2;

    // Room 4 - Armory
    strcpy(rooms[4].name, "Armory");
    strcpy(rooms[4].description, "Weapons and armory are displayed on racks, a suit of armor stands guard.");
    rooms[4].exits[0] = -1; // North blocked
    rooms[4].exits[1] = -1; // South blocked
    rooms[4].exits[2] = 1; // East to great hall
    rooms[4].exits[3] = -1; // West blocked
    strcpy(rooms[4].items[0], "sword");
    rooms[4].item_count = 1;

    // Room 5 - Secret Room
    strcpy(rooms[5].name, "Secret Room");
    strcpy(rooms[5].description, "A hidden chamber filled with glittering treasures and ancient artifacts.");
    rooms[5].exits[0] = -1; // North blocked
    rooms[5].exits[1] = -1; // South blocked
    rooms[5].exits[2] = 2; // East to library
    rooms[5].exits[3] = -1; // West blocked
    strcpy(rooms[5].items[0], "treasure");
    strcpy(rooms[5].items[1], "gem");
    rooms[5].item_count = 2;
}

// Display room, info presentation, environment feedback, location details
// available items and navigation options for players.
void display_room(void) {
    Room *current = &rooms[player.current_room];

    printf("\n --- %s ---\n", current->name);
    printf("%s\n", current->description);

    // Display items
    if (current->item_count > 0) {
        printf("\nItems here: ");
        for (int i = 0; i < current->item_count; i++) {
            printf("%s", current->items[i]);
            if (i < current->item_count - 1) printf(", ");
        }
        printf("\n");
    }

    // Show available exits with directions
    printf("\nExits");
    char *directions[] = {"North", "South", "East", "West"};
    int exit_found = 0;

    for (int i = 0; i < 4; i++) {
        if (current->exits[i] != -1) {
            if (exit_found) printf(", ");
            printf("%s", directions[i]);
            exit_found = 1;
        }
    }
    if (!exit_found) printf("None");

    // Display player status
    printf("\n\nHealth: %d | Score: %d | Items: %d/5\n", player.health, player.score, player.item_count);
}

// Command processor for multiple input formats to achieve robust user interaction
void process_command(char *command) {
    for (int i = 0; command[i]; i++) {
        if (command[i] >= 'A' && command[i] <= 'Z') {
            command[i] = command[i] + 32;
        }
    }

    // Movement commands for the player with support for different commands
    if (strcmp(command, "north") == 0 || strcmp(command, "n") == 0) {
        move_player(0);
    }
    else if (strcmp(command, "south") == 0 || strcmp(command, "s") == 0) {
        move_player(1);
    }
    else if (strcmp(command, "east") == 0 || strcmp(command, "e") == 0) {
        move_player(2);
    }
    else if (strcmp(command, "west") == 0 || strcmp(command, "w") == 0) {
        move_player(3);
    }

    // Item interaction
    else if (strncmp(command, "take ", 5) == 0) {
        take_item(command + 5);
    }
    else if (strncmp(command, "get ", 4) == 0) {
        take_item(command + 4);
    }
    // Utility and info commands
    else if (strcmp(command, "inventory") == 0 || strcmp(command, "i") == 0) {
        show_inventory();
    }
    else if (strcmp(command, "help") == 0) {
        show_help();
    }
    else if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
        game_over();
    }
    // Invalid command, helpful feedback
    else {
        printf("Invalid command. Type 'help' for available commands.\n");
    }
}

// PLAYER MOVEMENT
void move_player(int direction) {
    Room *current = &rooms[player.current_room];

    // Movement possibility
    if (current->exits[direction]  == -1) {
        printf("You cannot go there.\n");
        return;
    }

    // Otherwise execute movement
    player.current_room = current->exits[direction];
    printf("You moved to the %s.\n", rooms[player.current_room].name);

    // Exploration points for new areas
    player.score += 10;

    // Check for treasure room discovery
    if (player.current_room == 5) {
        printf("\nCongratulations! You've discovered the secret treasure room!\n");
        player.score += 100;
    }
}

// Item acquiring and inventory management
void take_item(char *item_name) {
    Room *current = &rooms[player.current_room];

    // Check inventory capacity
    if (player.item_count >= 5) {
        printf("Your inventory is full! You cannot fit more items!\n");
        return;
    }

    // Search for the requested item in the current room
    for (int i = 0; i < current->item_count; i++) {
        if (strcmp(current->items[i], item_name) == 0) {
            // Add item to inventory
            strcpy(player.inventory[player.item_count], item_name);
            player.item_count++;

            // Remove item from room by shifting array elements
            for (int j = i; j < current->item_count - 1; j++) {
                strcpy(current->items[j], current->items[j + 1]);
            }
            current->item_count--;

            printf("You took the %s.\n", item_name);
            player.score += 5; // Points for item collection
            return;
        }
    }

    printf("There is no %s here\n", item_name);
}

// INVENTORY DISPLAY
void show_inventory(void) {
    printf("\n=== INVENTORY ===\n");

    if (player.item_count == 0) {
        printf("Your inventory is empty.\n");
    } else {
        printf("You are carrying:\n");
        for (int i = 0; i < player.item_count; i++) {
            printf("- %s\n", player.inventory[i]);
        }
    }
    printf("Capacity: %d/5 items\n", player.item_count);
}

// HELP COMMAND SYSTEM
void show_help(void) {
    printf("\n=== Available Commands ===\n");
    printf("Movement:\n");
    printf("north or n = Move to north\n");
    printf("south or s = Move to south\n");
    printf("east or e = Move to east\n");
    printf("west or w, Move to west\n");
    printf("\nItem interaction\n");
    printf("take <item> - Pick up an item\n");
    printf("get <item> - Alternative way to pick up an item\n");
    printf("inventory or i = Show your items\n");
    printf("\nOthers\n");
    printf("help = Show this help message\n");
    printf("quit, exit = End the game\n");
    printf("\nGoal of the game: Explore the castle, collect items, and find the secret treasure!\n");
}

// GAME TERMINATION
void game_over(void) {
    printf("\n=== GAME OVER ===\n");
    printf("Thanks for playing the Castle Adventure\n");
    printf("Final Score: %d points\n", player.score);
    printf("Items Collected: %d\n", player.item_count);

    if (player.item_count > 0) {
        printf("Your final inventory:\n");
        for (int i = 0; i < player.item_count; i++) {
            printf("- %s\n", player.inventory[i]);
        }
    }    
    printf("Come back anytime to explore the castle again!\n");
    game_running = 0; // Terminate main game loop
}