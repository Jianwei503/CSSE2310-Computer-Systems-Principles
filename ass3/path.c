#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"

/*
 * Load and initialize a path from a given file
 *
 * pathName: a file name where load path from
 * path: a pointer where store sites to
 *
 * returns: true if load path successfully, false if not
 */
bool load_path(FILE *file, Path *path) {
    int sitesCount = 0, value;
    char site[4] = {'\0'};
    value = fscanf(file, "%d%c", &sitesCount, site);
    if ((value != 2) || sitesCount < MIN_PATH_SIZE || site[0] != ';') {
        return false;
    }
    path->sitesCount = sitesCount;
    path->sites = (Site *)malloc(sizeof(Site) * sitesCount);
    memset(path->sites, 0, sizeof(Site) * sitesCount);
    value = fscanf(file, "%c%c%c", site, site + 1, site + 2);
    if ((value != 3) || !is_barrier(site)) {
        clean_path(path);
        return false;
    }
    path->sites[0].name[0] = site[0];
    path->sites[0].name[1] = site[1];
    path->sites[0].capacity = site[2];
    for (int i = 1; i < sitesCount - 1; ++i) {
        value = fscanf(file, "%c%c%c", site, site + 1, site + 2);
        if ((value != 3) || !is_valid_site(site)) {
            clean_path(path);
            return false;
        }
        ((path->sites) + i)->name[0] = site[0];
        ((path->sites) + i)->name[1] = site[1];
        ((path->sites) + i)->capacity = site[2];
    }
    value = fscanf(file, "%c%c%c", site, site + 1, site + 2);
    if ((value != 3) || !is_barrier(site)) {
        clean_path(path);
        return false;
    }
    ((path->sites) + sitesCount - 1)->name[0] = site[0];
    ((path->sites) + sitesCount - 1)->name[1] = site[1];
    ((path->sites) + sitesCount - 1)->capacity = site[2];
    if (fgetc(file) != '\n') { //|| fgetc(file) != EOF
        clean_path(path);
        return false;
    }
    for (int i = 0; i < sitesCount; ++i) {
        set_barrier(i, path);
    }
    return true;
}

/*
 * Set up isBarrier according to site name
 *
 * index: index(in sites list) of site which we are setting up
 * path: a pointer of path which we are setting up
 */
void set_barrier(int index, Path *path) {
    if ((path->sites[index].name[0] == ':')
            && (path->sites[index].name[1] == ':')
            && (path->sites[index].capacity == '-')) {
        path->sites[index].isBarrier = true;
    } else {
        path->sites[index].isBarrier = false;
    }
}

/*
 * Verify a given site is a barrier site or not
 *
 * siteType: a site's type
 * returns: true if given site is a barrier, false if not
 */
bool is_barrier(const char *siteType) {
    if ((strncmp(siteType, "::-", 3) == 0)) {
        return true;
    }
    return false;
}

/*
 * Verify a given site is a valid site type or not
 *
 * siteType: a site's type
 * returns: true if given site is a valid site, false if not
 */
bool is_valid_site(const char *siteType) {
    if (is_barrier(siteType)) {
        return true;
    }
    if (((strncmp(siteType, "Mo", 2) == 0)
            || (strncmp(siteType, "V1", 2) == 0)
            || (strncmp(siteType, "V2", 2) == 0)
            || (strncmp(siteType, "Do", 2) == 0)
            || (strncmp(siteType, "Ri", 2) == 0))
            && (siteType[2] >= '1')
            && (siteType[2] <= '9')) {
        return true;
    }
    return false;
}

/*
 * Release memory held by path->sites, initialize path parameters
 *
 * path: a path which is going to be initialized
 */
void clean_path(Path *path) {
    if (path->sites) {
        free(path->sites);
        path->sites = 0;
    }
    path->sitesCount = 0;
}

/*
 * Load and initialize a board for containing players' ID
 *
 * sitesCount: number of sites in path
 * playersCount: number of players
 * board: a pointer where store the board to
 *
 * returns: true if load board successfully, false if not
 */
bool load_board(int sitesCount, int playersCount, Board *board) {
    board->board = (int *)malloc(sizeof(int) * sitesCount * playersCount);
    if (board->board == NULL) {
        return false;
    }
    board->sitesCount = sitesCount;
    board->playersCount = playersCount;
    for (int i = 0; i < playersCount; ++i) {
        for (int j = 0; j < sitesCount; ++j) {
            board->board[i * sitesCount +j] = 99;
        }
    }
    int maxID = playersCount - 1;
    for (int i = 0; i < playersCount; ++i) { // load players' ID
        board->board[i * sitesCount] = maxID--;
    }
    return true;
}

/*
 * Push the playerID up to the most top empty space,
 * along the given column (siteNumber)
 * Verify whether the given column is full before pushing
 *
 * playerID: the player ID which to be pushed
 * siteNumber: the number of site where player to go
 * board: a pointer of board which we are handling
 */
void move_to(int playerID, int siteNumber, Board *board) {
//    TO_2D(board->sitesCount)board->board;
    for (int i = 0; i < board->playersCount; ++i) {
        if (board->board[i * board->sitesCount + siteNumber] == VACANCY) {
            board->board[i * board->sitesCount + siteNumber] = playerID;
            break;
        }
    }
}

/*
 * Delete a playerID which at the lowest position,
 * along the given column (siteNumber)
 *
 * siteNumber: the number of site where player was at
 * board: a pointer of board which we are handling
 */
void delete_from(int siteNumber, Board *board) {
    int bottom = board->playersCount - 1;
    for (int i = bottom; i >= 0; --i) {
        if (board->board[i * board->sitesCount + siteNumber] != VACANCY) {
            board->board[i * board->sitesCount + siteNumber] = VACANCY;
            break;
        }
    }
}

/*
 * Display the path information to specified output
 *
 * path: the sites list to be printed
 * board: the board to be printed
 * stream: the output end where print information to
 */
void print_path(Path *path, Board *board, FILE *stream) {
    //max number of lines we are gonna print for board
    int max = 0;
    for (int i = 0; i < path->sitesCount; ++i) { // print sites
        max = (path->sites[i].population > max)
                ? path->sites[i].population : max;
        fprintf(stream, "%c%c%c", path->sites[i].name[0],
                path->sites[i].name[1], ' ');
    }
    fprintf(stream, "%c", '\n');
    int width = board->sitesCount;
    for (int i = 0; i < max; ++i) { // print board
        for (int j = 0; j < width; ++j) {
            int value = board->board[i * width + j];
            if (value == VACANCY) {
                fprintf(stream, "%3c", ' ');
            } else {
                fprintf(stream, "%-3d", value);
            }
        }
        fprintf(stream, "%c", '\n');
    }
}

/*
 * Verify whether the specified site has room or not
 *
 * sitNumber: number of the specified site
 * path: sites list
 *
 * returns: true if has room, else false
 */
bool has_room(int siteNumber, Path *path) {
    if (path->sites[siteNumber].isBarrier) {
        return true;
    }
    return ((CHAR_TO_INT(path->sites[siteNumber].capacity))
            - path->sites[siteNumber].population) > 0;
}