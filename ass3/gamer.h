#ifndef A3_GAMER_H
#define A3_GAMER_H

#include "path.h"
#include "player.h"

typedef enum {
    G_OK = 0,
    G_BAD_ARGS = 1,
    G_BAD_COUNT = 2,
    G_BAD_ID = 3,
    G_BAD_PATH = 4,
    G_EARLY_END = 5,
    G_COMM_ERR = 6
} GamerStatus;

typedef enum {
    YT,
    EARLY,
    DONE,
    HAP,
    ERR
} Message;

typedef struct {
    int playerID;
    int playersCount;
    Path *path;
    Board *board;
    Player *playersHub;
} Gamer;

Gamer gamer; // a global handler for child process
extern int pick_site(void);
void block_sigpipe(void);
void print_scores(void);
void player_go_to(int id, int newSite);
GamerStatus show_message(GamerStatus status);
GamerStatus initialize_gamer(int id, int playersCount);
GamerStatus run_game();
GamerStatus send_move(char *message);
GamerStatus update_move(char *message);
GamerStatus process_game_over(char *message);
GamerStatus process_early_end(char *message);
Message analyze_message(char *message);

#endif //A3_GAMER_H
