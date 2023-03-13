#include "utilities.h"

/**
 * Checks whether a given string contains the given character or not
 * @param string: the string being checked
 * @return true if the character was not found in the string,
 *         otherwise return false
 */
bool is_valid_argument(const char *string) {
    if (string == NULL) {
        return false;
    }
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '\n' || string[i] == '\r' || string[i] == ':') {
            return false;
        }
    }
    return true;
}

/**
 * Checks whether a given string is a valid port or not
 * @param string: the string being checked
 * @return true if the given string is a valid port number
 *         false if not
 */
bool is_valid_port(const char *string) {
    if (string == NULL) {
        return false;
    }
    int i, port;
    for (i = 0; string[i] != 0; ++i) {
        if (!isdigit(string[i])) {
            return false;
        }
    }
    port = atoi(string);
    if (port <= 0 || port > MAX_PORT) {
        return false;
    }
    return true;
}

/**
 * Reads a line of messages from given file descriptor
 * @param fileDescriptor: where the messages being read from
 * @param source: a buffer that stores the read messages
 * @param maxLength: max length for each line of messages
 * @return the number of characters has been read ('\n' exclusive)
 *         0 if read nothing
 *         -1 if read error or invalid message format
 */
ssize_t read_line(int fileDescriptor, void *source, size_t maxLength) {
    ssize_t number, count;
    char character, *pointer;
    pointer = source;
    for (number = 1; number < maxLength; ++number) {
        if ((count = read(fileDescriptor, &character, sizeof(char))) == 1) {
            if (character == '\n') {
                break;
            }
            *pointer++ = character;
        } else if (count == 0) {
            *pointer = 0;
            return number - 1;
        } else {
            return -1;
        }
    }
    *pointer = 0;
    return number - 1;
}
