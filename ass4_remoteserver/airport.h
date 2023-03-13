#ifndef A4_AIRPORT_H
#define A4_AIRPORT_H

#include "utilities.h"

/**
 * Exit conditions for airport program
 */
typedef enum {
    A_OK,
    BAD_NUM,
    BAD_CHAR,
    BAD_PORT,
    BAD_CONN
} AirportStatus;

Airport *airport; // a global handler for instance of airport program

/**
 * Outputs error message according to the given status
 * @param status: exit condition
 * @return AirportStatus
 */
AirportStatus show_message(AirportStatus status);

/**
 * Initializes the instance of struct Airport
 * holding by airport server
 */
void init_airport(int argc, char **argv);

/**
 * Connects to mapper server, registers this
 * airport server's ID and port number
 * @return AirportStatus
 */
AirportStatus connect_to_mapper(void);

/**
 * Sends back a newline separated list of all the planes
 * which have visited this airport
 * @param client: a pointer of client instance
 */
void send_plane_logs(Client *client);

/**
 * Sends back this airport's info terminated by a newline
 * @param client: a pointer of client instance
 */
void send_airport_info(Client *client);

/**
 * Stores a plane's ID which has visited this airport into logHub,
 * and records total number of planes which have visited the airport
 * @param id: ID of a plane which visited this airport
 */
void log_plane_id(const char *id);

#endif //A4_AIRPORT_H
