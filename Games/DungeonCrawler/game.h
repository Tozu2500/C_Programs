#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ROOMS 6
#define MAX_INVENTORY 5
#define MAX_NAME_LEN 50
#define MAX_DESC_LEN 200

// Game's structures
typedef struct {
    char name[MAX_NAME_LEN];
    char description[MAX_DESC_LEN];
    int damage;
    int healing;
} Item;

typedef struct {
    char name[MAX_NAME_LEN];
    char description[MAX_DESC_LEN];
    Item items[3];
    int item_count;
    int north, south, east, west;
    int visited;
    int has_enemy;
    int enemy_defeated;
} Room;

typedef struct {
    int health;
    int max_health;
    Item inventory[MAX_INVENTORY];
    int inventory_count;
    int current_room;
    int has_key;
} Player;

void init_game(Player *player, Room rooms[]);
void print_room(Room *room, Player *player);
void process_command(char *command, Player *player, Room rooms[]);
void move_player(char direction, Player *player, Room rooms[]);
void take_item(char *item_name, Player *player, Room *room);
void use_item(char *item_name, Player *player);
void show_inventory(Player *player);
void show_help(void);
void combat(Player *player);
int game_over_check(Player *player, Room rooms[]);
void print_intro(void);
void print_victory(void);

#endif // GAME_H