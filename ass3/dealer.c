#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <wait.h>
#include <math.h>
#include <limits.h>
#include <fcntl.h>
#include "dealer.h"

/*
 * Output error message for the given status
 *
 * status: error status
 * returns: given status
 */
DealerStatus show_messages(DealerStatus status) {
    char *message = "";
    switch (status) {
        case D_OK:
            return D_OK;
        case D_BAD_ARGS:
            message = "Usage: 2310dealer deck path p1 {p2}";
            break;
        case D_BAD_DECK:
            message = "Error reading deck";
            break;
        case D_BAD_PATH:
            message = "Error reading path";
            break;
        case D_START_ERR:
            message = "Error starting process";
            break;
        case D_COMM_ERR:
            message = "Communications error";
            break;
        default:
            message = "Unknown error";
    }
    fprintf(stderr, "%s\n", message);
    return status;
}

/*
 * Signal handler for SIGHUP
 */
void get_hang_upped(int signal) {
    (void)signal;
    dealer.hangUpped = true;
    exit(9);
}

/*
 * Set up ignoring SIGPIPE signal
 */
void block_sigpipe(void) {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_IGN;
    action.sa_flags = SA_RESTART;
    sigaction(SIGPIPE, &action, 0);
}

/*
 * Set up signal handler for SIGHUP
 */
void set_sighup(void) {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = get_hang_upped;
    action.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &action, 0);
}

/*
 * Exit hook which kills child processes when exit
 */
void do_exit_action(void) {
    if (dealer.hangUpped) {
        fprintf(stderr, "Ended due to signal\n");
        for (int i = 0; i < dealer.playersCount; ++i) {
            if ((dealer.hub + i)->pid != 0) {
                //terminate player process
                kill((dealer.hub + i)->pid, 9);
            }
            //recycle exit status, release remaining occupied memory
            waitpid((dealer.hub + i)->pid, 0, 0);
        }
    }
}

/*
 *Set up hub initial values
 *
 * argc: arguments' count
 * argv: given arguments
 */
void initialize_dealer(int argc, char **argv) {
    dealer.playersCount = argc - 3;
    dealer.hangUpped = false;
    dealer.path->sites[0].population = dealer.playersCount;
    dealer.hub = malloc(sizeof(HubMember) * dealer.playersCount);
    for (int i = 0; i < dealer.playersCount; ++i) {
        dealer.hub[i].player = (Player *)malloc(sizeof(Player));
        dealer.hub[i].player->hand = malloc(sizeof(Hand) * HAND_SIZE);
        dealer.hub[i].pid = 0;
        dealer.hub[i].fileName = argv[3 + i];
        load_player(dealer.hub[i].player, i);
    }
}

/*
 * Release the memories used by start_process
 */
void clean_arguments(char **argv) {
    for (int i = 1; i < 3; ++i) {
        free(argv[i]);
    }
    free(argv);
}

/*
 * Release the memories used the game
 */
void clean_game(void) {
    free(dealer.path->sites);
    free(dealer.path);
    free(dealer.deck->cards);
    free(dealer.deck);
    free(dealer.board->board);
    free(dealer.board);
    for (int i = 0; i < dealer.playersCount; ++i) {
        free(dealer.hub[i].player->hand);
        free(dealer.hub[i].player);
    }
    free(dealer.hub);
}

/*
 * Move the given player from current site to new site
 * Make sure call player_go_to() prior to calling update_player_info()
 *
 * id: ID of player who is going to move
 * newSite: number of new site where to go
 */
void player_go_to(int id, int newSite) {
    int atSite = dealer.hub[id].player->atSite;
    dealer.hub[id].player->atSite = newSite;
    --(dealer.path->sites[atSite].population);
    ++(dealer.path->sites[newSite].population);
    move_to(id, newSite, dealer.board);
    delete_from(atSite,dealer.board);
    if (dealer.path->sites[newSite].name[0] == 'V') {
        if (dealer.path->sites[newSite].name[1] == '1') {
            ++(dealer.hub[id].player->v1Count);
        } else {
            ++(dealer.hub[id].player->v2Count);
        }
    }
}

/*
 * Start all players processes
 *
 * returns: false if any of players processes did not start properly,
 *          else true
 */
bool start_process(void) {
    bool result = true;
    char **arguments = malloc(sizeof(char *) * 4); // 1space for '\0'
    for (int i = 1; i < 3; ++i) {
        arguments[i] = malloc(sizeof(char) * (log10(UINT_MAX) + 2));
    }
    arguments[3] = 0; // a terminator for function: execvp()
    sprintf(arguments[1], "%d", dealer.playersCount);
    for (int i = 0; i < dealer.playersCount; ++i) {
        arguments[0] = dealer.hub->fileName;
        sprintf(arguments[2], "%d", i);
        int hubToPlayer[2];
        int playerToHub[2];
        bool started = false;
        if (!pipe(hubToPlayer) && !pipe(playerToHub)) {
            pid_t pid = fork();
            if (pid < 0) {         // fork error
            } else if (pid == 0) { // child process
                dup2(hubToPlayer[0], 0);
                dup2(playerToHub[1], 1);
                close(hubToPlayer[1]);
                close(playerToHub[0]);
                int fd = open("/dev/null", S_IWUSR); //O_WRONLY
                dup2(fd, 2); // squash player process stderr
                execvp(arguments[0], arguments);
                _exit(99);
            } else { // parent process
                dealer.hub[i].pid = pid;
                dealer.hub[i].to = fdopen(hubToPlayer[1], "a");
                dealer.hub[i].from = fdopen(playerToHub[0], "r");
                close(hubToPlayer[0]);
                close(playerToHub[1]);
                started = (fgetc(dealer.hub[i].from) == '^');
            }
        }
        // if any player process did not start, kill all other processes
        if (!started) {
            for (int j = 0; j < i; ++j) {
                kill(dealer.hub[j].pid, SIGKILL);
            }
            result = false;
            break;
        }
    }
    clean_arguments(arguments);
    return result;
}

/*
 * Send characters in path file to each player
 *
 * fileName: name of file that contains path info
 */
void send_path(char *fileName) {
    int chars;
    FILE *file = fopen(fileName, "r");
    while ((chars = fgetc(file)) != EOF) {
        for (int i = 0; i < dealer.playersCount; ++i) {
            fprintf(dealer.hub[i].to, "%c", chars);
        }
    }
    for (int i = 0; i < dealer.playersCount; ++i) {
        fflush(dealer.hub[i].to);
    }
    fclose(file);
}

/*
 * Run a complete dealer task
 *
 * fileName: name of path file
 *
 * returns: dealer status
 */
DealerStatus run_game(char *fileName) {
    send_path(fileName);
    print_path(dealer.path, dealer.board, stdout);
    while (!game_over()) {
        int id = get_next();
        if (id == -1) {
            return D_OK;
        }
        fprintf(dealer.hub[id].to, "YT\n");
        fflush(dealer.hub[id].to);
        char *message;
        int length = read_line(dealer.hub[id].from, &message);
        int site = verify_reply(length, message, id); // new site number
        if (site == -1) {
            for (int i = 0; i < dealer.playersCount; ++i) {
                fprintf(dealer.hub[i].to, "EARLY\n");
                fflush(dealer.hub[i].to);
            }
            return D_COMM_ERR;
        }
        free(message);
        int *buff = handle_reply(id, site);
        for (int i = 0; i < dealer.playersCount; ++i) {
            fprintf(dealer.hub[i].to, "HAP%d,%d,%d,%d,%d\n",
                    id, site, buff[0], buff[1], buff[2]);
            fflush(dealer.hub[i].to);
        }
        player_go_to(id, site);
        update_player_info(dealer.hub[id].player, id, site,
                buff[0], buff[1], buff[2]);
        print_player(dealer.hub[id].player, stdout);
        print_path(dealer.path, dealer.board, stdout);
        free(buff);
    }
    for (int i = 0; i < dealer.playersCount; ++i) {
        fprintf(dealer.hub[i].to, "DONE\n");
        fflush(dealer.hub[i].to);
    }
    print_scores();
    return D_OK;
}

/*
 * Print out the scores in players' order at the end of game
 */
void print_scores(void) {
    int score;
    int count = dealer.playersCount;
    printf("Scores:%c", ' ');
    for (int i = 0; i < count - 1; ++i) {
        score = get_score(dealer.hub[i].player);
        printf("%d,", score);
    }
    score = get_score(dealer.hub[count - 1].player);
    printf("%d\n", score);
}

/*
 * Take an action according to site type that player are going
 * Mo: gain 3 money
 * Do: convert all money into points, p = m / 2
 * Ri: draw a card from deck
 *
 * id: ID of player who sent message
 * siteNumber: number of site where is going to
 *
 * returns: a pointer that stores 3 int, change of (points, money, card)
 */
int *handle_reply(int id, int siteNumber) {
    int point;
    int *buffer = malloc(sizeof(int) * 3);
    switch (dealer.path->sites[siteNumber].name[0]) {
        case 'M':
            buffer[0] = 0;
            buffer[1] = 3;
            buffer[2] = 0;
            break;
        case 'D':
            point = dealer.hub[id].player->money / 2;
            buffer[0] = point;
            buffer[1] = -dealer.hub[id].player->money;
            buffer[2] = 0;
            break;
        case 'R':
            buffer[0] = 0;
            buffer[1] = 0;
            char card = draw_card(dealer.deck);
            buffer[2] = card - 64;
            break;
        default:
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
    }
    return buffer;
}

/*
 * Verify whether the message replied by player is valid or not
 *
 * length: the length of message
 * message: the message sent by player
 * id: ID of player who sent message
 *
 * returns: number of site where is going to if it is valid, else -1
 */
int verify_reply(int length, char *message, int id) {
    if (length == 0 && feof(dealer.hub[id].from)) {
        return -1;
    }
    if (strncmp(message, "DO", strlen("DO")) != 0) {
        return -1;
    }
    message += 2;
    char *error;
    int siteNumber = (int)strtol(message, &error, 10);
    if (*error != '\0' || siteNumber < 0
            || siteNumber >= dealer.path->sitesCount) {
        return -1;
    }
    if (!has_room(siteNumber, dealer.path)) {
        return -1;
    }
    for (int i = dealer.hub[id].player->atSite + 1;
            i < siteNumber; ++i) {
        if (dealer.path->sites[i].isBarrier) {
            return -1;
        }
    }
    return siteNumber;
}

/*
 * Get the ID of player who takes the next turn to move
 *
 * returns: this player's ID
 */
int get_next(void) {
    int value;
    int width = dealer.board->sitesCount;
    for (int j = 0; j < width; ++j) {
        for (int i = dealer.playersCount - 1; i >= 0; --i) {
            value = dealer.board->board[i * width +j];
            if (value != VACANCY) {
                return value;
            }
        }
    }
    return -1;
}

/*
 * Verify whether the game is finished normally
 *
 * returns: true if game finished normally, false if is not finished yet
 */
bool game_over(void) {
    bool over = true;
    for (int i = 0; i < dealer.path->sitesCount - 1; ++i) {
        if (dealer.path->sites[i].population != 0) {
            over = false;
            break;
        }
    }
    return over;
}

int main(int argc, char **argv) {
    DealerStatus status = D_OK;
    if (argc < 4) {
        return show_messages(D_BAD_ARGS);
    }
    dealer.deck = (Deck *)malloc(sizeof(Deck));
    if (!load_deck(argv[1], dealer.deck)) {
        return show_messages(D_BAD_DECK);
    }
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        return show_messages(D_BAD_PATH);
    }
    dealer.path = (Path *)malloc(sizeof(Path));
    if (!load_path(file, dealer.path)) {
        return show_messages(D_BAD_PATH);
    }
    initialize_dealer(argc, argv);
    dealer.board = (Board *)malloc(sizeof(Board));
    if (!load_board(dealer.path->sitesCount,
            dealer.playersCount, dealer.board)) {
        return show_messages(D_BAD_PATH);
    }
    block_sigpipe();
    atexit(do_exit_action);
    set_sighup();
    if (!start_process()) {
        status = D_START_ERR;
    } else {
        status = run_game(argv[2]);
    }
    clean_game();
    return show_messages(status);
}
