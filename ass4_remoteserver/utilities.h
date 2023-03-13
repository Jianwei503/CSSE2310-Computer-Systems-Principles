#ifndef A4_UTILITIES_H
#define A4_UTILITIES_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_PORT 65535 // maximum valid port number
#define MAX_TEXT_LEN 80
#define MAX_PORT_LEN 6 // one char space for '\0'
#define MAX_QUEUE 10   // maximum number of ports that
                       // server listening on at one time
#define SYS_SPECIFIED_PORT 0

/**
 * A struct of one logHub information
 *
 * record: one record information of logHub
 */
typedef struct {
    char *record;
} LogInfo;

/**
 * Information of an client's program
 *
 * socket: an end for client to communicate with server
 */
typedef struct {
    int socket;
} Client;

/**
 * Information of an airport
 *
 * name: ID of the airport
 * selfPort: port number of the airport
 * logCount: number of entries of messages in logHub
 * connectFailed: whether connecting to mapper failed or not
 */
typedef struct {
    char *name;
    char *info;
    int selfPort;
    int mapperPort;
    int logCount;
    LogInfo *logHub;
    bool connectFailed;
    sem_t semaphore;
} Airport;

/**
 * Checks whether a given string contains the given character or not
 * @param string: the string being checked
 * @return true if the character was not found in the string,
 *         otherwise return false
 */
bool is_valid_argument(const char *string);

/**
 * Checks whether a given string is a valid port or not
 * @param string: the string being checked
 * @return true if the given string is a valid port number
 *         false if not
 */
bool is_valid_port(const char *string);

/**
 * Reads a line of messages from given file descriptor
 * @param fileDescriptor: where the messages being read from
 * @param source: a buffer that stores the read messages
 * @param maxLength: max length for each line of messages
 * @return the number of characters has been read ('\n' exclusive)
 *         0 if read nothing
 *         -1 if read error or invalid message format
 */
ssize_t read_line(int fileDescriptor, void *source, size_t maxLength);

/**
 * Starts a server side
 * via IPv4 TCP networking
 */
void start_server(void);

/**
 * Acts on any command the server received via connection
 * if client disconnected, this server thread would close the socket,
 * terminate and release the memory automatically
 * @param argument: a pointer of client instance
 * @return NULL
 */
void *run_request(void *argument);

/**
 * An server acts on corresponding command
 * @param command: the received command massage
 * @param client: a pointer of client instance
 */
void execute_command(const char *command, Client *client);

/**
 * Compares two instances of struct
 * @param first: the first instance of struct that will be compared
 * @param second: the second instance of struct that will be compared
 * @return 0 if 1st instance's string == 2nd instance's string
 *         1 if 1st instance's string > 2nd instance's string
 *         -1 if 1st instance's string < 2nd instance's string
 */
int compare(const void *first, const void *second);

#endif //A4_UTILITIES_H
