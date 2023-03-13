#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

/**
 * Checks whether a given number is an integer or not
 * @param number: the number that being checked
 * @return true if the number is an integer
 *         false if is not
 */
bool is_integer(const char *number) {
    if (number == NULL) {
        return false;
    }
    int i;
    for (i = 0; number[i] != 0; ++i) {
        if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Adds two numbers and prints the sum
 * @param first: the first number being calculated
 * @param second: the second number being calculated
 */
void add(const char *first, const char *second) {
    int one, two;
    one = atoi(first);
    two = atoi(second);
    printf("%d\n", one + two);
}

/**
 * The 1st number subtracts the 2nd number and prints the difference
 * @param first: the first number being calculated
 * @param second: the second number being calculated
 */
void minus(const char *first, const char *second) {
    int one, two;
    one = atoi(first);
    two = atoi(second);
    printf("%d\n", one - two);
}

/**
 * Multiplies two numbers and prints the product
 * @param first: the first number being calculated
 * @param second: the second number being calculated
 */
void times(const char *first, const char *second) {
    int one, two;
    one = atoi(first);
    two = atoi(second);
    printf("%d\n", one * two);
}

/**
 * The 1st number modules the 2nd number and prints the mode
 * @param first: the first number being calculated
 * @param second: the second number being calculated
 */
void module(const char *first, const char *second) {
    int one, two;
    one = atoi(first);
    two = atoi(second);
    printf("%d\n", one % two);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        puts("ERR");
        return 0;
    }
    if (!is_integer(argv[1]) || !is_integer(argv[3])) {
        puts("ERR");
        return 0;
    }
    if (strcmp(argv[2], "+") != 0 && strcmp(argv[2], "-") != 0 &&
            strcmp(argv[2], "*") != 0 && strcmp(argv[2], "%") != 0) {
        puts("ERR");
        return 0;
    }
    if (strcmp(argv[2], "%") == 0 && strcmp(argv[3], "0") == 0) {
        puts("NaN");
        return 0;
    }
    if (strcmp(argv[2], "+") == 0) {
        add(argv[1], argv[3]);
    } else if (strcmp(argv[2], "-") == 0) {
        minus(argv[1], argv[3]);
    } else if (strcmp(argv[2], "*") == 0) {
        times(argv[1], argv[3]);
    } else {
        module(argv[1], argv[3]);
    }
    return 0;
}
