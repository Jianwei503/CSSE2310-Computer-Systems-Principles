#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define AMOUNT 58 //26 lower letters + 26 upper letters + 6 punctuations
#define BUFFER 80

/**
 * Checks whether two different words are anagram words ro not
 * @param one the first word being checked
 * @param two the second word being checked
 * @return true if two words are anagram words
 *         false if are not
 */
bool is_anagram(const char *one, const char *two) {
    int stringOne[AMOUNT] = {0};
    int stringTwo[AMOUNT] = {0};
    int i = 0;
    if (strcmp(one, two) == 0) { //a word is not anagram word to itself
        return false;
    }
    while (one[i] != 0) {
        stringOne[one[i] - 'A']++;
        ++i;
    }
    i = 0;
    while (two[i] != 0) {
        stringTwo[two[i] - 'A']++;
        ++i;
    }
    for (i = 0; i < AMOUNT; ++i) {
        if (stringOne[i] != stringTwo[i]) {
            return false;
        }
    }
    return true;
}

/**
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

int main(int argc, char **argv) {
    if (argc != 3) {
        return 0;
    }
    for (int i = 0; i < strlen(argv[1]); ++i) {
        if (!isalpha(argv[1][i])) {
            return 0;
        }
    }
    for (int i = 0; i < strlen(argv[2]); ++i) {
        if (!isalpha(argv[2][i])) {
            return 0;
        }
    }
    char *buff;
    while (read_line(stdin, &buff) != 0) {
        bool s1Anagram = is_anagram(buff, argv[1]);
        bool s2Anagram = is_anagram(buff, argv[2]);

        if (s1Anagram && s2Anagram) {
            puts("12");
        } else if (s1Anagram) {
            puts("1");
        } else if (s2Anagram) {
            puts("2");
        }
        free(buff);
        buff = NULL;
    }
    return 0;
}