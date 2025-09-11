#include "game.h"

int main() {
    Player player;
    Room rooms[MAX_ROOMS];
    char command[100];

    srand(time(NULL));

    printf("\n=== Castle Escape ===\n");
    print_intro();

    init_game(&player, rooms);

    printf("\nType 'help' for commands.\n\n");

    while (1) {
        print_room(&rooms[player.current_room], &player);

        printf("\n> ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) continue;

        if (strcmp(command, "quit") == 0) {
            printf("Thanks for playing Castle Escape!\n");
            break;
        }

        process_command(command, &player, rooms);

        if (game_over_check(&player, rooms)) {
            break;
        }
    }

    return 0;
}