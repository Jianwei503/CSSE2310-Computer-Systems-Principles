#ifndef A3_DEALER_H
#define A3_DEALER_H

#include <unistd.h>
#include "player.h"
#include "deck.h"

typedef enum {
    D_OK = 0,
    D_BAD_ARGS = 1,
    D_BAD_DECK = 2,
    D_BAD_PATH = 3,
    D_START_ERR = 4,
    D_COMM_ERR = 5
} DealerStatus;

typedef struct {
    FILE *to;
    FILE *from;
    pid_t pid;
    char *fileName; //name of child process
    Player *player;
} HubMember;

typedef struct {
    Path *path;
    Deck *deck;
    Board *board;
    int playersCount;
    HubMember *hub;
    bool hangUpped;
} Dealer;

Dealer dealer; //a global handler for dealer process
DealerStatus show_messages(DealerStatus status);
DealerStatus run_game(char *fileName);
void initialize_dealer(int argc, char **argv);
void get_hang_upped(int signal);
void block_sigpipe(void);
void set_sighup(void);
void do_exit_action(void);
bool start_process(void);
void clean_arguments(char **argv);
void clean_game(void);
void player_go_to(int id, int newSite);
bool game_over(void);
int get_next(void);
int verify_reply(int length, char *message, int id);
int *handle_reply(int id, int siteNumber);
void print_scores(void);
void send_path(char *fileName);

#endif //A3_DEALER_H
