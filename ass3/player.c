#include <stdio.h>
#include <stdlib.h>
#include "player.h"

/*
 * Initialize player's information
 *
 * player: the player who is going to be initialized
 */
void load_player(Player *player, int playerID) {
    player->playerID = playerID;
    player->money = 7;
    player->points = 0;
    player->atSite = 0;
    player->v1Count = 0;
    player->v2Count = 0;
    for (int i = 0; i < HAND_SIZE; ++i) {
        player->hand[i].suit = 65 + i;
        player->hand[i].count = 0;
    }
}

/*
 *Display the information of the player who moved
 *
 * player: a pointer of player who moved
 * stream: the output end where print information to
 */
void print_player(Player *player, FILE *stream) {
    fprintf(stream, "Player %d Money=%d V1=%d V2=%d "
                    "Points=%d A=%d B=%d C=%d D=%d E=%d\n",
                    player->playerID, player->money, player->v1Count,
                    player->v2Count, player->points, player->hand[0].count,
                    player->hand[1].count, player->hand[2].count,
                    player->hand[3].count, player->hand[4].count);
}

/*
 * Update player's information using the given parameters
 * Make sure call player_fo_to() before calling the function
 *
 * p: player ID
 * n: new site for player
 * s: additional points for player
 * m: change in money for player (could be < 0)
 * c: card drawn by player (0,1='A',...5='E')
 *
 * returns: 0 if updated properly, else returns -1
 */
int update_player_info(Player *player, int p, int n, int s, int m, int c) {
    if (p != player->playerID) {
        return -1;
    }
    player->atSite = n;
    player->points += s;
    player->money += m;
    if (c < 0 || c > 5) {
        return -1;
    }
    if ((c >= 1) && (c <= 5)) {
        ++(player->hand[c - 1].count);
    }
    return 0;
}

/*
 * Count how many card types are there in a player's hand
 *
 * player: a player who has this hand
 * returns: types number
 */
int count_hand_types(Player *player) {
    int count = 0;
    for (int i = 0; i < 5; ++i) {
        if ((player->hand + i)->count > 0) {
            ++count;
        }
    }
    return count;
}

/*
 * Count total score for a player at the end of game
 *
 * player: a player whom count the final score for
 *
 * returns: total score
 */
int get_score(Player *player) {
    int score = 0;
    while (count_hand_types(player) == 5) {
        for (int i = 0; i < 5; ++i) {
            --((player->hand + i)->count);
        }
        score += 10;
    }
    for (int i = 4; i > 0; --i) {
        while (count_hand_types(player) == i) {
            for (int j = 0; j < 5; ++j) {
                --((player->hand + j)->count);
            }
            score += (i * 2) - 1;
        }
    }
    return score + player->points + player->v1Count + player->v2Count;
}

/*
 * Read a line from a given pointer, store it into a buffer
 * buffer does not have '\n'
 *
 * file: file pointer to read from
 * buffer: buffer pointer, used to store chars
 *
 * returns: length of string(chars + '\0')
 */
int read_line(FILE *file, char **buffer) {
    *buffer = malloc(sizeof(char) * BUFFER);
    int size = BUFFER; // max number of chars can be stored
    int count = 0;     // number of chars that have been read
    int character = fgetc(file);
    while (character != EOF && character != '\n') {
        (*buffer)[count++] = character;
        if (count == size - 1) { // check for buffer overflow
            size = 2 * size;
            *buffer = realloc(*buffer, sizeof(char) * size);
        }
        character = fgetc(file);
    }
    (*buffer)[count] = 0;
    return count;
}