#ifndef A3_PLAYER_H
#define A3_PLAYER_H

#include "path.h"

#define HAND_SIZE 5
#define BUFFER 80

typedef struct {
    char suit;
    int count;
} Hand; //cards drawn by a player

typedef struct {
    int playerID;
    int money;
    int points;
    int atSite;
    int v1Count;
    int v2Count;
    Hand *hand;
} Player;

void load_player(Player *player, int playerID);
void print_player(Player *player, FILE *stream);
int count_hand_types(Player *player);
int get_score(Player *player);
int read_line(FILE *file, char **buffer);
int update_player_info(Player *player, int p, int n, int s, int m, int c);

#endif //A3_PLAYER_H
