#ifndef A3_PATH_H
#define A3_PATH_H

#include <stdio.h>
#include <stdbool.h>

#define CHAR_TO_INT(ch) ch - '0'
#define MIN_PATH_SIZE 2
#define VACANCY 99

typedef struct {
    char name[2];
    char capacity;
    int population; //how many players at this site currently
    bool isBarrier;
} Site;

typedef struct {
    int sitesCount;
    Site *sites;
} Path;

typedef struct {
    int sitesCount;
    int playersCount;
    int *board;
} Board;

bool load_path(FILE *file, Path *path);
bool is_barrier(const char *siteType);
bool is_valid_site(const char *siteType);
void clean_path(Path *path);
void set_barrier(int index, Path *path);
void print_path(Path *path, Board *board, FILE *stream);
bool load_board(int sitesCount, int playersCount, Board *board);
void move_to(int playerID, int siteNumber, Board *board);
void delete_from(int siteNumber, Board *board);
bool has_room(int siteNumber, Path *path);

#endif //A3_PATH_H
