#include "aircraft.h"

/**
 * Outputs error message according to the given status
 * @param status: exit condition
 * @return PlaneStatus
 */
PlaneStatus show_message(PlaneStatus status) {
    char *message = "";
    switch (status) {
        case OK:
            return OK;
        case BAD_ARG_NUM:
            message = "Usage: roc2310 id mapper {airports}";
            break;
        case BAD_MAP_PORT:
            message = "Invalid mapper port";
            break;
        case NO_MAP_PORT:
            message = "Mapper required";
            break;
        case BAD_MAP_CONN:
            message = "Failed to connect to mapper";
            break;
        case NO_MAP_ENTRY:
            message = "No map entry for destination";
            break;
        case BAD_AIRPORT_CONN:
            message = "Failed to connect to at least one destination";
            break;
        default:
            message = "Unknown error";
    }
    fprintf(stderr, "%s\n", message);
    return status;
}

/**
 * Initializes the instance of struct Plane
 * holding by plane client
 * @param argc: number of arguments
 * @param argv: values of arguments
 */
void init_plane(int argc, char **argv) {
    plane = malloc(sizeof(Plane));
    assert(plane != NULL);
    plane->name = argv[1];
    plane->mapPort = argv[2];
    plane->logCount = 0;
    plane->log = NULL;
    plane->destinationCount = argc - 3;
    plane->needMapper = false;
    plane->connectFailed = false;
    plane->airportIdCount = 0;
    memset(plane->indexes, 0, 20 * sizeof(int));
}

/**
 * Connects to an airport server using specified port, sends
 * this plane's ID to airport server, receives airport's info
 * @param index: index of specified port in arguments
 * @param argv: values of arguments
 */
void connect_to_airport(int index, char **argv) {
    struct sockaddr_in server;
    int planeSock, link;
    planeSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (planeSock < 0) {
        plane->connectFailed = true;
        return;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[index]));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    link = connect(planeSock, (struct sockaddr *)&server, sizeof(server));
    if (link < 0) {
        close(planeSock);
        plane->connectFailed = true;
        return;
    }
    FILE *send = fdopen(dup(planeSock), "w+");
    fprintf(send, "%s\n", plane->name);
    fflush(send);
    fclose(send);
    shutdown(planeSock, SHUT_WR);
    while (1) {
        char *reply = malloc(MAX_TEXT_LEN * sizeof(char));
        assert(reply != NULL);
        memset(reply, 0, MAX_TEXT_LEN * sizeof(char));
        ssize_t count = read_line(planeSock, reply, MAX_TEXT_LEN);
        if (count == 0) { // end of airport info
            free(reply);
            break;
        } else if (count < 0 || !is_valid_argument(reply)) {
            free(reply);
            break;
        }
        ++(plane->logCount);
        plane->log = realloc(plane->log, plane->logCount * sizeof(LogInfo));
        assert(plane->log != NULL);
        plane->log[plane->logCount - 1].record = reply;
    }
    close(planeSock);
}

/**
 * Connects to mapper server, queries the unknown airports's ports,
 * then stores the ports to corresponding arguments
 * @param argv: values of arguments
 * @return PlaneStatus
 */
PlaneStatus connect_to_mapper(char **argv) {
    struct sockaddr_in server;
    int planeSock, link;
    planeSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (planeSock < 0) {
        return BAD_MAP_CONN;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(plane->mapPort));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    link = connect(planeSock, (struct sockaddr *)&server, sizeof(server));
    if (link < 0) {
        return BAD_MAP_CONN;
    }
    FILE *send = fdopen(dup(planeSock), "w+");
    for (int i = 0; i < plane->airportIdCount; ++i) { // sending commands
        fprintf(send, "?%s\n", argv[plane->indexes[i]]);
        fflush(send);
    }
    shutdown(planeSock, SHUT_WR);
    for (int i = 0; i < plane->airportIdCount; ++i) {
        char *replay = malloc(MAX_PORT_LEN * sizeof(char));
        assert(replay != NULL);
        memset(replay, 0, MAX_PORT_LEN * sizeof(char));
        read_line(planeSock, replay, MAX_PORT_LEN);
        if (strcmp(replay, ";") == 0 || strcmp(replay, "") == 0) {
            return NO_MAP_ENTRY; // strcmp(""): in case of invalid airportID
        }
        argv[plane->indexes[i]] = replay;
    }
    fclose(send);
    close(planeSock);
    return OK;
}

/**
 * Finds out and counts how many airports' IDs there are in arguments,
 * stores each ID's index into plane->indexes
 * @param argc: number of arguments
 * @param argv: values of arguments
 */
void find_airports_id(int argc, char **argv) {
    int count = 0;
    for (int i = 3; i < argc; ++i) {
        if (!is_valid_port(argv[i])) {
            plane->needMapper = true;
            plane->indexes[count] = i;
            ++count;
        }
    }
    plane->airportIdCount = count;
}

int main(int argc, char **argv) {
    PlaneStatus status = OK;
    if (argc < 3) {
        return show_message(BAD_ARG_NUM);
    }
    if (strcmp(argv[2], "-") != 0 && !is_valid_port(argv[2])) {
        return show_message(BAD_MAP_PORT);
    }
    init_plane(argc, argv);
    find_airports_id(argc, argv);
    if (plane->needMapper && !is_valid_port(plane->mapPort)) {
        return show_message(NO_MAP_PORT);
    }
    if (plane->needMapper) {
        if ((status = connect_to_mapper(argv)) != OK) {
            return show_message(status);
        }
    }
    for (int i = 3; i < argc; ++i) {
        connect_to_airport(i, argv);
    }
    for (int i = 0; i < plane->logCount; ++i) {
        printf("%s\n", plane->log[i].record);
        free(plane->log[i].record);
    }
    free(plane->log);
    if (plane->connectFailed) {
        free(plane);
        return show_message(BAD_AIRPORT_CONN);
    }
    free(plane);
    return show_message(status);
}