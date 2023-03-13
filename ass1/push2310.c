#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//convert 1d array to 2d array by using the pointer(newGame) of game
//(width * 2 + 1) -> stride of 2d array, (game2D) -> pointer of the 2d array
#define GAME_TO_2D_ARRAY(width) char (*game2D)[width * 2 + 1] = (void *)game
#define CHAR_TO_INT(character) character - '0'
#define SIZE_OF_GAME rows * (columns * 2 + 1) * sizeof(char)
#define WIDTH columns * 2 //width of game board

typedef enum {
    NORMAL = 0,
    WRONG_ARGS_NUM = 1,
    WRONG_PLAYER_TYPE = 2,
    NO_LOAD_FILE = 3,
    WRONG_CONTENTS = 4,
    END_FILE = 5,
    FULL_BOARD = 6
} Status;

Status show_message(Status s) {
    const char *messages[] = {
            "",
            "Usage: push2310 typeO typeX fname\n",
            "Invalid player type\n",
            "No file to load from\n",
            "Invalid file contents\n",
            "End of file\n",
            "Full board in load\n"};
    fputs(messages[s], stderr);
    return s;
}

static char nextPlayer;   //the player is to have the next turn
static int coordinate[2]; //a coordinate where robot1 play stone

//declaration of functions
int get_columns(char *fileName);
int get_rows(char *fileName);
void swap(char *x, char *y);
int is_number(char character);
char get_next_player(char *fileName);
char *number_to_string(int number, char *string);
void find_winner(int rows, int columns, char *game);
int are_valid_cells(int rows, int columns, char *game);
void display_board(int rows, int columns, char *game);
int is_full_board(int rows, int columns, char *game);
char *robot0_of_O_move(int rows, int columns, char *game);
char *robot0_of_X_move(int rows, int columns, char *game);
void find_highest_value(int rows, int columns, char *game);
int score(char player, int rows, int columns, char *game);
void save_game(char *name, int rows, int columns, char *game);
int out_of_boundary(int coordR, int coordC, int rows, int columns);
int load_game_board(char *fileName, int rows, int columns, char *game);
void analyse_address(char player, char *addr, int columns, char *game);
void push_right(int coordR, int columns, char player, char *game);
void push_left(int coordR, int columns, char player, char *game);
void push_up(int coordC, int rows, int columns, char player, char *game);
void push_down(int coordC, int rows, int columns, char player, char *game);
void reload_temp_board(int rows, int columns, char *game, char *tempGame);
int can_play_on_left(int coordR, int coordC, int columns, char *game);
int can_play_on_right(int coordR, int coordC, int columns, char *game);
bool one_empty_cell(int coordR, int coordC, int rows, int columns, char *game);
int can_play_on_top(int coordR, int coordC, int rows, int columns, char *game);
int can_play_interior(int coordR, int coordC, int rows, int columns, char *game);
int can_play_on_bottom(int coordR, int coordC, int rows, int columns, char *game);
void robot1_move(int rows, int columns, char player, char *game, char *tempGame);
void player_O_move(char playerType, int rows, int columns, char *game, char *tempGame);
void player_X_move(char playerType, int rows, int columns, char *game, char *tempGame);
int human_move(int coordR, int coordC, int rows, int columns, char player, char *game);
int lower_score(int coordR, int coordC, int rows, int columns, char player, char rival, char *game);

int main(int argc, char *argv[]) {
    char *game = NULL;
    char *tempGameO = NULL;
    char *tempGameX = NULL;
    if (argc != 4) {
        exit(show_message(WRONG_ARGS_NUM));
    }
    if (**(argv + 1) != '0' && **(argv + 1) != '1' && **(argv + 1) != 'H') {
        exit(show_message(WRONG_PLAYER_TYPE));
    }
    if (**(argv + 2) != '0' && **(argv + 2) != '1' && **(argv + 2) != 'H') {
        exit(show_message(WRONG_PLAYER_TYPE));
    }

    const int ROWS = get_rows(argv[3]);
    const int COLUMNS = get_columns(argv[3]);
    nextPlayer = get_next_player(argv[3]);

    if (ROWS < 3 || COLUMNS < 3) {
        exit(show_message(WRONG_CONTENTS));
    }
    if (nextPlayer != 'O' && nextPlayer != 'X') {
        exit(show_message(WRONG_CONTENTS));
    }
    game = (char *)malloc(ROWS * (COLUMNS * 2 + 1) * sizeof(char));
    if (load_game_board(argv[3], ROWS, COLUMNS, game)) {
        if (is_full_board(ROWS, COLUMNS, game)) {
            return show_message(FULL_BOARD);
        }
        if (!are_valid_cells(ROWS, COLUMNS, game)) {
            exit(show_message(WRONG_CONTENTS));
        }
        display_board(ROWS, COLUMNS, game);
    }
    if (**(argv + 1) == '1') {
        tempGameO = (char *)malloc(ROWS * (COLUMNS * 2 + 1) * sizeof(char));
    }
    if (**(argv + 2) == '1') {
        tempGameX = (char *)malloc(ROWS * (COLUMNS * 2 + 1) * sizeof(char));
    }
    while (!is_full_board(ROWS, COLUMNS, game)) {
        if (nextPlayer == 'O') {
            player_O_move(**(argv + 1), ROWS, COLUMNS, game, tempGameO);
        } else {
            player_X_move(**(argv + 2), ROWS, COLUMNS, game, tempGameX);
        }
        display_board(ROWS, COLUMNS, game);
    }
    find_winner(ROWS, COLUMNS, game);
    free(tempGameO);
    free(tempGameX);
    free(game);
    return 0;
}

/*Get the number of columns of the game board.*/
int get_rows(char *fileName) {
    FILE *file = NULL;
    char character = 0; //NULL
    char *buffer = (char *)calloc(64, 1);
    int loop = 0;
    int rowNumber = 0;
    //if the saved file not exists, exit with error 3
    if ((file = fopen(fileName, "rt")) == NULL) {
        exit(show_message(NO_LOAD_FILE));
    }
    while (character != ' ') {
        character = fgetc(file);
        if (character == EOF) {
            exit(show_message(WRONG_CONTENTS));
        }
        buffer[loop] = character;
        ++loop;
    }
    for (int i = 0; i < loop - 1; ++i) {
        if (!is_number(buffer[i])) {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    fclose(file);
    rowNumber = atoi(buffer);
    free(buffer);
    return rowNumber;
}

/*Get the number of rows of the game board.*/
int get_columns(char *fileName) {
    FILE *file = fopen(fileName, "rt");
    char character = 0; //NULL
    char *buffer = (char *)calloc(64, 1);
    int loop = 0;
    int rowNumber = 0;
    while (character != ' ') { //skip the row number and space
        character = fgetc(file);
    }
    character = 0;
    //save the rest of digits of column number
    while (character != '\n') {
        character = fgetc(file);
        if (character == EOF || character == ' ') {
            exit(show_message(WRONG_CONTENTS));
        }
        buffer[loop] = character;
        ++loop;
    }
    for (int i = 0; i < loop - 1; ++i) {
        if (!is_number(buffer[i])) {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    fclose(file);
    rowNumber = atoi(buffer);
    free(buffer);
    return rowNumber;
}

/*Get the type of player who is going to have the next turn.*/
char get_next_player(char *fileName) {
    FILE *file = fopen(fileName, "rt");
    char letter = 0;
    //skip the 1st line of saved file
    while ((letter = fgetc(file)) != '\n') {
        if (letter == EOF) {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    //read the 2nd line of saved file
    if ((letter = fgetc(file)) == EOF) {
        exit(show_message(WRONG_CONTENTS));
    }
    fclose(file);
    return letter;
}

/* Generate the game board of a new game by
 * extracting all characters in saved file.
 * Return 1 if generated successfully, exit if error happened.*/
int load_game_board(char *fileName, int rows, int columns, char *game) {
    char character = 0;
    FILE *file = fopen(fileName, "rt");
    //skip the 1st line of saved file
    while ((character = fgetc(file)) != '\n') {
        if (character == EOF) {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    //skip the 2st line of saved file
    while ((character = fgetc(file)) != '\n') {
        if (character == EOF) {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    //starts from the 3rd line, extract the characters
    // of saved file, fill the new game board up
    for (int i = 0; i < SIZE_OF_GAME; ++i) {
        if ((character = fgetc(file)) == EOF) {
            exit(show_message(WRONG_CONTENTS));
        }
        *(game + i) = character;
    }
    GAME_TO_2D_ARRAY(columns);
    //if each line in board does not end with '\n', exit
    for (int i = 0; i < rows; ++i) {
        if (*(*(game2D + i) + WIDTH) != '\n') {
            exit(show_message(WRONG_CONTENTS));
        }
    }
    fclose(file);
    return 1;
}

/*Check if the cells on game board have valid values,
 * Return 1 if yes, 0 if not.*/
int are_valid_cells(int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    //check if interior cells have values between 1 and 9
    for (int i = 1; i < (rows - 1); ++i) {
        for (int j = 2; j < (WIDTH - 2); j += 2) {
            int cell = CHAR_TO_INT(*(*(game2D + i) + j));
            if (cell < 1 || cell > 9) {
                return 0;
            }
        }
    }
    //check if border cells have values of 0
    for (int i = 1; i < (rows - 1); ++i) {
        char head = **(game2D + i);
        char tail = *(*(game2D + i) + (WIDTH - 2));
        if (head != '0' || tail != '0') {
            return 0;
        }
    }
    for (int j = 2; j < (WIDTH - 2); j += 2) {
        char top = *(*game2D + j);
        char bottom = *(*(game2D + (rows - 1)) + j);
        if (top != '0' || bottom != '0') {
            return 0;
        }
    }
    return 1;
}

/*Output all characters on game board to stdout*/
void display_board(int rows, int columns, char *game) {
    for (int i = 0; i < SIZE_OF_GAME; ++i) {
        printf("%c", *(game + i));
    }
}

/*Determine if the game is over.
 *Return 1 if no empty space interior, otherwise return 0.*/
int is_full_board(int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    for (int i = 1; i < (rows - 1); ++i) {
        for (int j = 3; j < (WIDTH - 2); j += 2) {
            if (*(*(game2D + i) + j) == '.') {
                return 0;
            }
        }
    }
    return 1;
}

/*Player O places a stone on the game board,
 * player X takes the next turn*/
void player_O_move(char playerType, int rows, int columns, char *game, char *tempGame) {
    char *position = NULL; //the address where robot 0 place stone
    int row = 0;           //the coordinate row number player entered
    int column = 0;        //the coordinate column number player entered
    int totalNums = 0;     //the total number of params player entered
    int notValidCoord = 1;
    char *inputs, *fileName;
    switch (playerType) {
        case '0':
            if ((position = robot0_of_O_move(rows, columns, game)) != NULL) {
                analyse_address('O', position, columns, game);
            }
            nextPlayer = 'X';
            break;
        case '1':
            robot1_move(rows, columns, 'O', game, tempGame);
            printf("Player O placed at %d %d\n", coordinate[0], coordinate[1]);
            nextPlayer = 'X';
            break;
        case 'H':
            inputs = (char *)calloc(90, 1);
            fileName = (char *)calloc(90, 1);
            while (notValidCoord) {
                memset(inputs, '\0', 89);
                memset(fileName, '\0', 89);
                printf("O:(R C)> ");
                fgets(inputs, 90, stdin);
                totalNums = sscanf(inputs, "%d %d", &row, &column);
                if (totalNums == 2
                        && !out_of_boundary(row, column, rows, columns)
                        && human_move(row, column, rows, columns, 'O', game)) {
                    notValidCoord = 0;
                } else if (inputs[0] == 's' && inputs[1] == '\n') {
                    memset(inputs,'\0',89);
                    fgets(inputs, 90, stdin);
                    sscanf(inputs, "%s", fileName);
                    save_game(fileName, rows, columns, game);
                } else if (inputs[0] == 's') {
                    sscanf(inputs, "%*[s]%s", fileName);
                    save_game(fileName, rows, columns, game);
                } else if (strlen(inputs) == 0) { //Ctrl + D
                    exit(show_message(END_FILE));
                }
            }
            nextPlayer = 'X';
            free(inputs);
            free(fileName);
            break;
    }
}

/*Player X places a stone on the game board,
 * player O takes the next turn.*/
void player_X_move(char playerType, int rows, int columns, char *game, char *tempGame) {
    char *position = NULL; //the address where robot 0 place stone
    int row = 0;           //the coordinate row number player entered
    int column = 0;        //the coordinate column number player entered
    int totalNums = 0;     //the total number of params palyer entered
    int notValidCoord = 1;
    char *inputs, *fileName;

    switch (playerType) {
        case '0':
            if ((position = robot0_of_X_move(rows, columns, game)) != NULL) {
                analyse_address('X', position, columns, game);
            }
            nextPlayer = 'O';
            break;
        case '1':
            robot1_move(rows, columns, 'X', game, tempGame);
            printf("Player X placed at %d %d\n", coordinate[0], coordinate[1]);
            nextPlayer = 'O';
            break;
        case 'H':
            inputs = (char *)calloc(90, 1);
            fileName = (char *)calloc(90, 1);
            while (notValidCoord) {
                memset(inputs, '\0', 89);
                memset(fileName, '\0', 89);
                printf("X:(R C)> ");
                fgets(inputs, 90, stdin);
                totalNums = sscanf(inputs, "%d %d", &row, &column);
                if (totalNums == 2
                        && !out_of_boundary(row, column, rows, columns)
                        && human_move(row, column, rows, columns, 'X', game)) {
                    notValidCoord = 0;
                } else if (inputs[0] == 's' && inputs[1] == '\n') {
                    memset(inputs,'\0',89);
                    fgets(inputs, 90, stdin);
                    sscanf(inputs, "%s", fileName);
                    save_game(fileName, rows, columns, game);
                } else if (inputs[0] == 's') {
                    sscanf(inputs, "%*[s]%s", fileName);
                    save_game(fileName, rows, columns, game);
                } else if (strlen(inputs) == 0) { //Ctrl + D
                    exit(show_message(END_FILE));
                }
            }
            nextPlayer = 'O';
            free(inputs);
            free(fileName);
            break;
    }
}

/*Find an available space for Player O whose type is 0 to place a stone in.
 *Search from top left to bottom right of the interior of game board.
 *Return the pointer of this space if could find one, otherwise return NULL.*/
char *robot0_of_O_move(int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    for (int i = 1; i < (rows - 1); ++i) {
        for (int j = 3; j < (WIDTH - 2); j += 2) {
            char *position = *(game2D + i) + j;
            if (*position == '.') {
                *position = 'O';
                return position;
            }
        }
    }
    return NULL;
}

/*Find an available space for Player X whose type is 0 to place a stone in.
 *Search from bottom right to top left of the interior of game board.
 *Return the pointer of this space if could find one, otherwise return NULL.*/
char *robot0_of_X_move(int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    for (int i = (rows - 2); i > 0; --i) {
        for (int j = (WIDTH - 3); j > 2; j -= 2) {
            char *position = *(game2D + i) + j;
            if (*position == '.') {
                *position = 'X';
                return position;
            }
        }
    }
    return NULL;
}

/*Check if can lower other's score by pushing stone on border,
 * left to right, top to down, check clockwise
 * if can't, play stone interior with highest score.*/
void robot1_move(int rows, int columns, char player, char *game, char *tempGame) {
    char rival;
    coordinate[0] = 0;
    coordinate[1] = 0;
    if (player == 'O') {
        rival = 'X';
    } else {
        rival = 'O';
    }
    for (int j = 1; j < columns - 1; ++j) { //check top border
        reload_temp_board(rows, columns, game, tempGame);
        if (can_play_on_top(0, j, rows, columns, tempGame)
                && lower_score(0, j, rows, columns, player, rival, tempGame)
                && !one_empty_cell(0, j, rows,columns,game)) {
            push_down(j, rows, columns, player, game);
            coordinate[0] = 0;
            coordinate[1] = j;
            return;
        }
    }
    for (int i = 1; i < rows - 1; ++i) { //check right border
        reload_temp_board(rows, columns, game, tempGame);
        if (can_play_on_right(i, columns - 1, columns, tempGame)
                && lower_score(i, columns - 1, rows,
                        columns, player, rival, tempGame)
                && !one_empty_cell(i, columns - 1, rows,columns,game)) {
            push_left(i, columns, player, game);
            coordinate[0] = i;
            coordinate[1] = columns - 1;
            return;
            }
    }
    for (int j = columns - 2; j > 0; --j) { //check bottom border
        reload_temp_board(rows, columns, game, tempGame);
        if (can_play_on_bottom(rows - 1, j, rows, columns, tempGame)
                && lower_score(rows - 1, j, rows, columns,
                        player, rival, tempGame)
                && !one_empty_cell(rows - 1, j, rows, columns, game)) {
            push_up(j, rows, columns, player, game);
            coordinate[0] = rows - 1;
            coordinate[1] = j;
            return;
        }
    }
    for (int i = rows - 2; i > 0; --i) { //check left border
        reload_temp_board(rows, columns, game, tempGame);
        if (can_play_on_left(i, 0, columns, tempGame)
                && lower_score(i, 0, rows, columns, player, rival, tempGame)
                && !one_empty_cell(i, 0, rows, columns,game)) {
            push_right(i, columns,player,game);
            coordinate[0] = i;
            coordinate[1] = 0;
            return;
        }
    }
    find_highest_value(rows, columns, game);
    GAME_TO_2D_ARRAY(columns);
    *(*(game2D + coordinate[0]) + (2 * coordinate[1] + 1)) = player;
}

/*Check if can lower rival's score after pushing action.
 *Return 1 if can, 0 is can not.*/
int lower_score(int coordR, int coordC, int rows, int columns,
        char player, char rival, char *game) {
    int scoreBefore = 0;
    int scoreAfter = 0;
    scoreBefore = score(rival, rows, columns, game);
    if (coordR == 0) {
        push_down(coordC, rows, columns, player, game);
    } else if (coordC == columns - 1) {
        push_left(coordR, columns, player, game);
    } else if (coordR == rows - 1) {
        push_up(coordC, rows, columns, player, game);
    } else if (coordC == 0) {
        push_right(coordR, columns, player, game);
    }
    scoreAfter = score(rival, rows, columns, game);
    if (scoreBefore > scoreAfter) {
        return 1;
    } else {
        return 0;
    }
}

/*Reload the game board to temporary game.*/
void reload_temp_board(int rows, int columns, char *game, char *tempGame) {
    for (int i = 0; i < SIZE_OF_GAME; ++i) {
        *(tempGame + i) = *(game + i);
    }
}

/*Find the empty interior cell with the highest value. */
void find_highest_value(int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    char value = '0';
    for (int i = 1; i < (rows - 1); ++i) {
        for (int j = 3; j < (WIDTH - 2); j += 2) {
            if (*(*(game2D + i) + j) == '.'
                    && *(*(game2D + i) + j - 1) > value){
                coordinate[0] = i;
                coordinate[1] = (j - 1) / 2;
                value = *(*(game2D + i) + j - 1);
            }
        }
    }
}

/*Place a stone(player's name) on the given coordinate.
 * Return 1 if played successfully, 0 if not.*/
int human_move(int coordR, int coordC, int rows,
        int columns, char player, char *game) {
    GAME_TO_2D_ARRAY(columns);
    if (can_play_interior(coordR, coordC, rows, columns, game)) {
        *(*(game2D + coordR) + (coordC * 2 + 1)) = player;
        return 1;
    } else if (can_play_on_top(coordR, coordC, rows, columns, game)) {
        push_down(coordC, rows, columns, player, game);
        return 1;
    } else if (can_play_on_right(coordR, coordC, columns, game)) {
        push_left(coordR, columns, player, game);
        return 1;
    } else if (can_play_on_bottom(coordR, coordC, rows, columns, game)) {
        push_up(coordC, rows, columns, player, game);
        return 1;
    } else if (can_play_on_left(coordR, coordC, columns, game)) {
        push_right(coordR, columns, player, game);
        return 1;
    }
    return 0;
}

/*Check if the given coordinate is out of boundary.
 * Return 1 if it is, 0 if it is not.*/
int out_of_boundary(int coordR, int coordC, int rows, int columns) {
    if (coordR < 0 || coordR > rows - 1) {
        return 1;
    }
    if (coordC < 0 || coordC > columns - 1) {
        return 1;
    }
    //check if coordinate is one of the corners
    if ((coordR == 0 && coordC == 0)
            || (coordR == 0 && coordC == columns - 1)
            || (coordR == rows - 1 && coordC == 0)
            || (coordR == rows - 1 && coordC == columns - 1)) {
        return 1;
    }
    return 0;
}
/*Check if there is only one empty cell in a row or column,
 * Return True if yes, False if no.*/
bool one_empty_cell(int coordR, int coordC, int rows, int columns, char *game){
    GAME_TO_2D_ARRAY(columns);
    int count = 0;
    if (coordR == 0) {
        for (int i = 0; i < rows; ++i) {
            if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {
                ++count;
            }
        }
    }
    if (coordC == columns - 1) {
        for (int j = 1; j < WIDTH; j += 2) {
            if (*(*(game2D + coordR) + j) == '.') {
                ++count;
            }
        }
    }
    if (coordR == rows - 1) {
        for (int i = 0; i < rows; ++i) {
            if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {
                ++count;
            }
        }
    }
    if (coordC == 0) {
        for (int j = 1; j < WIDTH; j += 2) {
            if (*(*(game2D + coordR) + j) == '.') {
                ++count;
            }
        }
    }
    return (count == 1);
}

/*Check if a player can play on a specified top
 * border coordinate(coordR, coordC).
 *Return 1 if can play, 0 if can not.*/
int can_play_on_top(int coordR, int coordC, int rows, int columns, char *game) {
    //check if given coordinate is on top line of board
    if (coordR != 0) {
        return 0;
    }
    GAME_TO_2D_ARRAY(columns);
    int hasEmptyCell = 0, hasNeighbor = 0;
    for (int i = 2; i < rows; ++i) { //check if there is a empty cell
        if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {//top 3rd to bottom
            hasEmptyCell = 1;
        }
    }
    //check top 2nd cell in given column is not empty
    if (*(*(game2D + 1) + (coordC * 2 + 1)) != '.') {
        hasNeighbor = 1;
    }
    if (hasEmptyCell && hasNeighbor) {
        return 1;
    }
    return 0;
}

/*Check if a player can play on a specified
 * bottom border coordinate(coordR, coordC).
 *Return 1 if can play, 0 if can not.*/
int can_play_on_bottom(int coordR, int coordC, int rows, int columns, char *game) {
    if (coordR != rows - 1) {
        return 0;
    }
    GAME_TO_2D_ARRAY(columns);
    int hasEmptyCell = 0, hasNeighbor = 0;
    for (int i = rows - 3; i >= 0; --i) {
        if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {//bottom 3rd to top
            hasEmptyCell = 1;
        }
    }
    if (*(*(game2D + (rows - 2)) + (coordC * 2 + 1)) != '.') {
        hasNeighbor = 1;
    }
    if (hasEmptyCell && hasNeighbor) {
        return 1;
    }
    return 0;
}

/*Check if a player can play on a specified
 * left border coordinate(coordR, coordC).
 *Return 1 if can play, 0 if can not. */
int can_play_on_left(int coordR, int coordC, int columns, char *game) {
    if (coordC != 0) {
        return 0;
    }
    GAME_TO_2D_ARRAY(columns);
    int hasEmptyCell = 0, hasNeighbor = 0;
    for (int j = 5; j < WIDTH; j += 2) {
        if (*(*(game2D + coordR) + j) == '.') { //left 3rd to right border
            hasEmptyCell = 1;
        }
    }
    if (*(*(game2D + coordR) + 3) != '.') { //check the left 2nd cell
        hasNeighbor = 1;
    }
    if (hasEmptyCell && hasNeighbor) {
        return 1;
    }
    return 0;
}

/*Check if a player can play on a specified
 * right border coordinate(coordR, coordC).
 *Return 1 if can play, 0 if can not. */
int can_play_on_right(int coordR, int coordC, int columns, char *game) {
    if (coordC != columns - 1) {
        return 0;
    }
    GAME_TO_2D_ARRAY(columns);
    int hasEmptyCell = 0, hasNeighbor = 0;
    for (int j = WIDTH - 5; j > 0; j -= 2) {
        if (*(*(game2D + coordR) + j) == '.') { //right 3rd to left border
            hasEmptyCell = 1;
        }
    }
    if (*(*(game2D + coordR) + (WIDTH - 3)) != '.') { //check right 2nd cell
        hasNeighbor = 1;
    }
    if (hasEmptyCell && hasNeighbor) {
        return 1;
    }
    return 0;
}

/*Check if a player can play on a specified
 * interior coordinate(coordR, coordC).
 *Return 1 if can play, 0 if can not.*/
int can_play_interior(int coordR, int coordC, int rows, int columns, char *game) {
    if (coordR < 1 || coordR > rows - 2) {
        return 0;
    }
    if (coordC < 1 || coordC > columns - 2) {
        return 0;
    }
    GAME_TO_2D_ARRAY(columns);
    if (*(*(game2D + coordR) + (coordC * 2 + 1)) == '.') {
        return 1;
    }
    return 0;
}

/*Push a player's stone down into game board.*/
void push_down(int coordC, int rows, int columns, char player, char *game) {
    int rowNumber = 0; //row number of empty cell in given column
    GAME_TO_2D_ARRAY(columns);
    //find the first empty cell below 2nd top cell
    for (int i = 2; i < rows; ++i) {
        if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {
            rowNumber = i;
            break;
        }
    }
    //bottom to top, swap two adjacent cells' value
    for (int i = rowNumber; i > 0; --i) {
        char *lower = *(game2D + i) + (coordC * 2 + 1);     //lower cell
        char *upper = *(game2D + i - 1) + (coordC * 2 + 1); //upper cell
        swap(lower, upper);
    }
    //player place stone on top border
    swap(*game2D + (coordC * 2 + 1), &player);
    if (*(*(game2D + 1) + (coordC * 2 + 1)) == '.') {
        //push stone down to 2nd top row
        swap(*game2D + (coordC * 2 + 1),
                *(game2D + 1) + (coordC * 2 + 1));
    }
}

/*Push a player's stone up into game board.*/
void push_up(int coordC, int rows, int columns, char player, char *game) {
    int rowNumber = 0;
    GAME_TO_2D_ARRAY(columns);
    for (int i = rows - 3; i >= 0; --i) {
        if (*(*(game2D + i) + (coordC * 2 + 1)) == '.') {
            rowNumber = i;
            break;
        }
    }
    for (int i = rowNumber; i < rows - 1; ++i) {
        char *upper = *(game2D + i) + (coordC * 2 + 1);     //upper cell
        char *lower = *(game2D + i + 1) + (coordC * 2 + 1); //lower cell
        swap(lower, upper);
    }
    swap(*(game2D + (rows - 1)) + (coordC * 2 + 1), &player);
    if (*(*(game2D + (rows - 2)) + (coordC * 2 + 1)) == '.') {
        //push stone up to 2nd bottom row
        swap(*(game2D + (rows - 1)) + (coordC * 2 + 1),
                *(game2D + (rows - 2)) + (coordC * 2 + 1));
    }
}

/*Push a player's stone right into game board.*/
void push_right(int coordR, int columns, char player, char *game) {
    int colNumber = 0;
    GAME_TO_2D_ARRAY(columns);
    for (int j = 5; j < WIDTH; j += 2) {
        if (*(*(game2D + coordR) + j) == '.') {
            colNumber = j;
            break;
        }
    }
    for (int j = colNumber; j > 2; j -= 2) { //right to left, swap
        char *front = *(game2D + coordR) + (j - 2); //front cell
        char *rear = *(game2D + coordR) + j;        //rear cell
        swap(front, rear);
    }
    swap(*(game2D + coordR) + 1, &player); //play on left border
    //push stone right to 2nd left column
    if (*(*(game2D + coordR) + 3) == '.') {
        swap(*(game2D + coordR) + 1, *(game2D + coordR) + 3);
    }
}

/*Push a player's stone left into game board.*/
void push_left(int coordR, int columns, char player, char *game) {
    int colNumber = 0;
    GAME_TO_2D_ARRAY(columns);
    for (int j = WIDTH - 5; j > 0; j -= 2) {
        if (*(*(game2D + coordR) + j) == '.') {
            colNumber = j;
            break;
        }
    }
    for (int j = colNumber; j < WIDTH - 2; j += 2) { //left to right, swap
        char *front = *(game2D + coordR) + j;      //front cell
        char *rear = *(game2D + coordR) + (j + 2); //rear cell
        swap(front, rear);
    }
    swap(*(game2D + coordR) + (WIDTH - 1), &player); //play on right border
    //push stone left to 2nd right column
    if (*(*(game2D + coordR) + (WIDTH - 3)) == '.') {
        swap(*(game2D + coordR) + (WIDTH - 1),
                *(game2D + coordR) + (WIDTH - 3));
    }
}

/*Swap the values of x and y.*/
void swap(char *x, char *y) {
    *x = *x ^ *y;
    *y = *x ^ *y;
    *x = *x ^ *y;
}

/*Analysing where an automated player 0 placed the stone,
 * output the coordinate to monitor.*/
void analyse_address(char player, char *addr, int columns, char *game) {
    int row, column;
    row = (addr - game) / (WIDTH + 1);
    column = (((addr - game) % (WIDTH + 1)) - 1) / 2;
    printf("Player %c placed at %d %d\n", player, row, column);
}

/*Calculate total score for the given player.*/
int score(char player, int rows, int columns, char *game) {
    GAME_TO_2D_ARRAY(columns);
    int score = 0;
    for (int i = 1; i < (rows - 1); ++i) {
        for (int j = 3; j < (WIDTH - 2); j += 2) {
            char *stone = *(game2D + i) + j;
            if (*stone == player) {
                score += CHAR_TO_INT(*(stone - 1));
            }
        }
    }
    return score;
}

/*Save the game to local directory with given file name.
 * 1st line:      rowNumber columnNumber
 * 2nd line:      playerName
 * from 3rd line: gameBoard*/
void save_game(char *name, int rows, int columns, char *game) {
    FILE *file;
    int index = 0;
    char *buffer = (char *)calloc(64, 1);
    if ((file = fopen(name, "wt")) == NULL) {
        fputs("Save failed\n", stderr);
    }
    //save row number
    number_to_string(rows, buffer);
    while (buffer[index] != '\0') {
        fputc(buffer[index], file);
        ++index;
    }
    fputc(' ', file);
    //save column number
    memset(buffer, '\0', 63);
    number_to_string(columns, buffer);
    index = 0;
    while (buffer[index] != '\0') {
        fputc(buffer[index], file);
        ++index;
    }
    fputc('\n', file);
    //save next player's name
    fputc(nextPlayer, file);
    fputc('\n', file);
    //save game board
    for (int i = 0; i < SIZE_OF_GAME; ++i) {
        fputc(*(game + i), file);
    }
    fclose(file);
    free(buffer);
}

/*Find out the player with the highest score.*/
void find_winner(int rows, int columns, char *game) {
    int scoreO = score('O', rows, columns, game);
    int scoreX = score('X', rows, columns, game);
    if (scoreO > scoreX) {
        puts("Winners: O");
    } else if (scoreO == scoreX) {
        puts("Winners: O X");
    } else {
        puts("Winners: X");
    }
}

/*Check if a character is number.
 * Return positive integer if yes, 0 if no.*/
int is_number(char character) {
    return (character >= '0' && character <= '9');
}

/*Convert long numbers to string.
 * Return a pointer of this string.*/
char *number_to_string(int number, char *string) {
    int i = 0;      //index of array(string)
    do {
        string[i++] = number % 10 + 48;
        number /= 10;
    } while (number); //number == 0, loop stops
    string[i] = '\0';
    //symmetrically swap both sides of array
    for (int j = 0; j < i / 2; ++j) {
        swap(string + j, string + (i - 1 -j));
    }
    return string;
}