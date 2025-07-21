#include "snake.h"

int save_game(const Game *game) {
    FILE *file = fopen(SAVE_FILE, "wb");
    if (!file) {
        perror("Failed to open save file");
        return 0;
    }

    size_t written = fwrite(game, sizeof(Game), 1, file);
    fclose(file);

    return written == 1;
}

int load_game(Game *game) {
    FILE *file = fopen(SAVE_FILE, "rb");
    if (!file) {
        return 0; // Failed to open save file
    }

    size_t read = fread(game, sizeof(Game), 1, file);
    fclose(file);

    return read == 1;
}

void save_stats(const Stats *stats) {
    FILE *file = fopen(STATS_FILE, "wb");
    if (file) {
        fwrite(stats, sizeof(Stats), 1, file);
        fclose(file);
    }
}

Stats load_stats(void) {
    Stats stats = {0};
    FILE *file = fopen(STATS_FILE, "rb");
    if (file) {
        fread(&stats, sizeof(Stats), 1, file);
        fclose(file);
    }
    return stats;
}

void update_stats(Stats *stats, const Game *game) {
    if (game->game_over) {
        stats->games_played++;
        stats->total_score += game->score;

        if (game->score > stats->high_score) {
            stats->high_score = game->score;
        }

        stats->total_food_eaten += (game->snake.length - 3); // Initial length is 3
        stats->last_played = time(NULL);

        save_stats(stats);
    }
}