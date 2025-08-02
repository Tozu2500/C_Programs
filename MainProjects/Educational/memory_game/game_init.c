#include "game.h"

char symbols[] = "ABCDEFGHIJKLMNOPQRTSUVWXYZ123456";

void init_game(Game* game, Difficulty diff) {
    game->size = (int)diff;
    game->total_pairs = (game->size * game->size) / 2;
    game->matched_pairs = 0;
    game->moves = 0;

    int symbol_count = 0;
    for (int i = 0; i < game->size; i++) {
        for (int j = 0; j < game->size; j++) {
            game->board[i][j].symbol = symbols[symbol_count / 2];
            game->board[i][j].revealed = 0;
            game->board[i][j].matched = 0;
            symbol_count++;
        }
    }

    shuffle_cards(game);
}

void shuffle_cards(Game* game) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 1000; i++) {
        int r1 = rand() % game->size;
        int c1 = rand() % game->size;
        int r2 = rand() % game->size;
        int c2 = rand() % game->size;

        char temp = game->board[r1][c1].symbol;
        game->board[r1][c1].symbol = game->board[r2][c2].symbol;
        game->board[r2][c2].symbol = temp;
    }
}

