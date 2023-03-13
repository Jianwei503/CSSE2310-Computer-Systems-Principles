#ifndef A4_MAPPER_H
#define A4_MAPPER_H

#include "utilities.h"

/**
 * Commands received from connected port
 * QUERY: represents ?ID
 * ADD: represents !ID:PORT
 * LIST: represents @
 * ERR: represents invalid command
 */
typedef enum {
    QUERY,
    ADD,
    LIST,
    ERR
} Command;

/**
 * A struct of mapper, stores all airports and airports' airCount
 *
 * airHub: a container stores all airports' instances
 * airCount: number of airports logged in mapper
 * semaphore: semaphore for controlling accessing Mapper information
 */
typedef struct {
    Airport *airHub;
    int airCount;
    sem_t semaphore;
} Mapper;

Mapper *mapper; // a global handler for instance of mapper program

/**
 * Initializes the instance of struct Mapper
 * holding by mapper server
 */
void init_mapper(void);

/**
 * Determines which command the received massage is
 * @param command: the received massage
 * @return QUERY if command starts with ? and is valid
 *         ADD if command starts with ! and is valid
 *         LIST if command starts with @ and is valid
 *         ERR if command is invalid
 */
Command analyze_command(const char *command);

/**
 * Adds airport's ID and port number as an entry into mapper's log
 * @param command: the received command massage, contains ID and port
 */
void update_mapping(const char *command);

/**
 * Sends back the port number followed by a newline to client
 * if there is an entry corresponding to the given ID, or
 * sends back a ';' followed by a newline if there is no mapping
 * @param command: the received command massage, contains ID
 * @param client: a pointer of client instance
 */
void query_mapping(const char *command, Client *client);

/**
 * Prints the mappings to client, in lexicographic order of ID,
 * each entry ends with newline, ID and port number are separated
 * by a colon
 * @param client: a pointer of client instance
 */
void list_mapping(Client *client);

#endif //A4_MAPPER_H
