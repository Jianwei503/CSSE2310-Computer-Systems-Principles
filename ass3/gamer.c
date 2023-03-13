#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "gamer.h"

/*
 * Output error message for the given status
 *
 * status: error status
 * returns: given status
 */
GamerStatus show_message(GamerStatus status) {
    char *message = "";
    switch (status) {
        case G_OK:
            return G_OK;
        case G_BAD_ARGS:
            message = "Usage: player pcount ID";
            break;
        case G_BAD_COUNT:
            message = "Invalid player count";
            break;
        case G_BAD_ID:
            message = "Invalid ID";
            break;
        case G_BAD_PATH:
            message = "Invalid path";
            break;
        case G_EARLY_END:
            message = "Early game over";
            break;
        case G_COMM_ERR:
            message = "Communications error";
            break;
        default:
            message = "Unknown error";
    }
    fprintf(stderr, "%s\n", message);
    return status;
}

/*
 * Determine the type of message that was received
 * returns ERR if does not match a known message
 *
 * message: string that contains message from dealer
 *
 * returns: type of message
 */
Message analyze_message(char *message) {
    if (strncmp(message, "YT", strlen("YT")) == 0) {
        return YT;
    } else if (strncmp(message, "HAP", strlen("HAP")) == 0) {
        return HAP;
    } else if (strncmp(message, "EARLY", strlen("EARLY")) == 0) {
        return EARLY;
    } else if (strncmp(message, "DONE", strlen("DONE")) == 0) {
        return DONE;
    } else {
        return ERR;
    }
}

/*
 * Ignore SIGPIPE
 */
void block_sigpipe(void) {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_IGN;
    action.sa_flags = SA_RESTART;
    sigaction(SIGPIPE, &action, 0);
}

/*
 * Move the given player from current site to new site
 * Make sure call player_go_to() prior to calling update_player_info()
 *
 * id: ID of player who is going to move
 * newSite: number of new site where to go
 */
void player_go_to(int id, int newSite) {
    int atSite = gamer.playersHub[id].atSite;
    gamer.playersHub[id].atSite = newSite;
    --(gamer.path->sites[atSite].population);
    ++(gamer.path->sites[newSite].population);
    move_to(id, newSite, gamer.board);
    delete_from(atSite, gamer.board);
    if (gamer.path->sites[newSite].name[0] == 'V') {
        if (gamer.path->sites[newSite].name[1] == '1') {
            ++(gamer.playersHub[id].v1Count);
        } else {
            ++(gamer.playersHub[id].v2Count);
        }
    }
}

/*
 * Load and initialize gamer's information
 *
 * id: gamer's playerID
 * playersCount: number of players
 *
 * returns: G_OK if initialized properly
 *          G_BAD_PATH due to path issue
 */
GamerStatus initialize_gamer(int id, int playersCount) {
    gamer.playerID = id;
    gamer.playersCount = playersCount;
    gamer.playersHub = malloc(sizeof(Player) * playersCount);
    for (int i = 0; i < gamer.playersCount; ++i) {
        gamer.playersHub[i].hand = malloc(sizeof(Hand) * HAND_SIZE);
    }
    for (int i = 0; i < playersCount; ++i) {
        load_player(&(gamer.playersHub[i]), i);
    }
    gamer.path = (Path *)malloc(sizeof(Path));
    if (!load_path(stdin, gamer.path)) {
        return G_BAD_PATH;
    }
    gamer.board = (Board *)malloc(sizeof(Board));
    if (!load_board(gamer.path->sitesCount,
                    gamer.playersCount, gamer.board)) {
        return G_BAD_PATH;
    }
    gamer.path->sites[0].population = playersCount;
    return G_OK;
}

/*
 * Handles the message of 'YT', player picks a
 * new site to move in, sends it to dealer
 *
 * message: the message received from dealer
 *
 * returns: G_OK if sent a move properly
 *          G_COMM_ERR due to incorrect message format
 */
GamerStatus send_move(char *message) {
    message += strlen("YT");
    if (*message != '\0') {
        return G_COMM_ERR;
    }
    int newSite = pick_site();
    fprintf(stdout, "DO%d\n", newSite);
    fflush(stdout);
    return G_OK;
}

/*
 * Handles the message of 'HAP', player gos to new site,
 * updates info according to message
 * print this player's info and path to stderr
 *
 * message: the message received from dealer
 *
 * returns: G_OK if updated moved info properly
 *          G_COMM_ERR due to incorrect message info
 */
GamerStatus update_move(char *message) {
    int count, info[6] = {0};
    char end;
    message += strlen("HAP");
    count = sscanf(message, "%d%*[,]%d%*[,]%d%*[,]%d%*[,]%d%c",
            &info[0], &info[1], &info[2], &info[3], &info[4], &end);
    if (count != 5
            || info[0] < 0 || info[0] >= gamer.playersCount
            || info[1] < 0 || info[1] >= gamer.path->sitesCount
            || info[2] < 0
            || info[4] < 0 || info[4] > 5) {
        return G_COMM_ERR;
    }
    player_go_to(info[0], info[1]);
    update_player_info(&gamer.playersHub[info[0]], info[0],
            info[1], info[2], info[3], info[4]);
    print_player(&gamer.playersHub[info[0]], stderr);
    print_path(gamer.path, gamer.board, stderr);
    return G_OK;
}

/*
 * Handles the message of 'DONE', player picks a
 * new site to move in, sends it to dealer
 *
 * message: the message received from dealer
 *
 * returns: G_OK if message format is correct
 *          G_COMM_ERR if not
 */
GamerStatus process_game_over(char *message) {
    message += strlen("DONE");
    if (*message != '\0') {
        return G_COMM_ERR;
    }
    return G_OK;
}

/*
 * Handles the message of 'EARLY', player quit
 * the game without doing any thing
 *
 * message: the message received from dealer
 *
 * returns: G_EARLY_END if message format is correct
 *          G_COMM_ERR if not
 */
GamerStatus process_early_end(char *message) {
    message += strlen("EARLY");
    if (*message != '\0') {
        return G_COMM_ERR;
    }
    return G_EARLY_END;
}

/*
 * Print out the scores in players' order at the end of game
 */
void print_scores(void) {
    int score;
    int count = gamer.playersCount;
    fprintf(stderr, "Scores:%c", ' ');
    for (int i = 0; i < count - 1; ++i) {
        score = get_score(&gamer.playersHub[i]);
        fprintf(stderr, "%d,", score);
    }
    score = get_score(&gamer.playersHub[count - 1]);
    fprintf(stderr, "%d\n", score);
}

/*
 * Player play a complete game
 * Features main game loop
 * Will exit the game if EOF is received or
 * a message is invalid
 *
 * returns: gamer status
 */
GamerStatus run_game() {
    bool over = false; // flag for 'game over'
    print_path(gamer.path, gamer.board, stderr);
    while (!over) {
        char *message;
        int length = read_line(stdin, &message);
        if (length == 0 && feof(stdin)) {
            return G_COMM_ERR;
        }
        switch (analyze_message(message)) {
            case YT:
                if (send_move(message) != G_OK) {
                    return G_COMM_ERR;
                }
                break;
            case HAP:
                if (update_move(message) != G_OK) {
                    return G_COMM_ERR;
                }
                break;
            case DONE:
                if (process_game_over(message) != G_OK) {
                    return G_COMM_ERR;
                }
                over = true;
                break;
            case EARLY:
                if (process_early_end(message) != G_EARLY_END) {
                    return G_COMM_ERR;
                }
                return G_EARLY_END;
            case ERR:
                return G_COMM_ERR;
        }
    }
    print_scores();
    return G_OK;
}

int main(int argc, char **argv) {
    GamerStatus status = G_OK;
    if (argc != 3) {
        return show_message(G_BAD_ARGS);
    }
    char *error;
    int count = (int)strtol(argv[1], &error, 10);
    if (count < 1 || *error != '\0') {
        return show_message(G_BAD_COUNT);
    }
    int id = (int)strtol(argv[2], &error, 10);
    if (id < 0 || id >= count || *error != '\0' || strlen(argv[2]) == 0) {
        return show_message(G_BAD_ID);
    }
    block_sigpipe();
    putc('^', stdout);
    fflush(stdout);
    if ((status = initialize_gamer(id, count)) != G_OK) {
        return show_message(status);
    }
    status = run_game();
    return show_message(status);
}
