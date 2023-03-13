#ifndef A4_AIRCRAFT_H
#define A4_AIRCRAFT_H

#include "utilities.h"

/**
 * Exit conditions for aircraft program
 */
typedef enum {
    OK,
    BAD_ARG_NUM,
    BAD_MAP_PORT,
    NO_MAP_PORT,
    BAD_MAP_CONN,
    NO_MAP_ENTRY,
    BAD_AIRPORT_CONN
} PlaneStatus;

/**
 * Information of an aircraft
 *
 * name: ID of the airport
 * mapPort: port number of mapper server
 * connectFailed: the case of not being able to connect to destination
 * airportIdCount: number of airports' IDs in given arguments
 * indexes: an array that stores index of airport ID in arguments
 */
typedef struct {
    char *name;
    char *mapPort;
    int logCount;
    LogInfo *log;
    bool needMapper;
    bool connectFailed;
    int destinationCount;
    int airportIdCount;
    int indexes[20];
} Plane;

Plane *plane; // a global handler for instance of aircraft program

/**
 * Outputs error message according to the given status
 * @param status: exit condition
 * @return PlaneStatus
 */
PlaneStatus show_message(PlaneStatus status);

/**
 * Initializes the instance of struct Plane
 * holding by plane client
 * @param argc: number of arguments
 * @param argv: values of arguments
 */
void init_plane(int argc, char **argv);

/**
 * Finds out and counts how many airports' IDs there are in arguments,
 * stores each ID's index into plane->indexes
 * @param argc: number of arguments
 * @param argv: values of arguments
 */
void find_airports_id(int argc, char **argv);

/**
 * Connects to mapper server, queries the unknown airports's ports,
 * then stores the ports to corresponding arguments
 * @param argv: values of arguments
 * @return PlaneStatus
 */
PlaneStatus connect_to_mapper(char **argv);

/**
 * Connects to an airport server using specified port, sends
 * this plane's ID to airport server, receives airport's info
 * @param index: index of specified port in arguments
 * @param argv: values of arguments
 */
void connect_to_airport(int index, char **argv);

#endif //A4_AIRCRAFT_H
