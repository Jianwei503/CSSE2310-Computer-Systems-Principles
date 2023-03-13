#include "airport.h"

/**
 * Outputs error message according to the given status
 * @param status: exit condition
 * @return AirportStatus
 */
AirportStatus show_message(AirportStatus status) {
    char *message = "";
    switch (status) {
        case A_OK:
            return A_OK;
        case BAD_NUM:
            message = "Usage: control2310 id info [mapper]";
            break;
        case BAD_CHAR:
            message = "Invalid char in parameter";
            break;
        case BAD_PORT:
            message = "Invalid port";
            break;
        case BAD_CONN:
            message = "Can not connect to map";
            break;
        default:
            message = "Unknown error";
    }
    fprintf(stderr, "%s\n", message);
    return status;
}

/**
 * Initializes the instance of struct Airport
 * holding by airport server
 */
void init_airport(int argc, char **argv) {
    airport = malloc(sizeof(Airport));
    assert(airport != NULL);
    airport->name = argv[1];
    airport->info = argv[2];
    airport->selfPort = 0;
    airport->mapperPort = 0;
    airport->logCount = 0;
    airport->logHub = NULL;
    airport->connectFailed = false;
    sem_init(&airport->semaphore, 0, 1);
    if (argc == 4) {
        airport->mapperPort = atoi(argv[3]);
    }
}

/**
 * Compares two instances of struct
 * @param first: the first instance of struct that will be compared
 * @param second: the second instance of struct that will be compared
 * @return 0 if 1st instance's string == 2nd instance's string
 *         1 if 1st instance's string > 2nd instance's string
 *         -1 if 1st instance's string < 2nd instance's string
 */
int compare(const void *first, const void *second) {
    LogInfo *one = (LogInfo *)first;
    LogInfo *two = (LogInfo *)second;
    return strcmp(one->record, two->record);
}

/**
 * Sends back a newline separated list of all the planes
 * which have visited this airport
 * @param client: a pointer of client instance
 */
void send_plane_logs(Client *client) {
    sem_wait(&airport->semaphore);                    // critical region
    qsort(airport->logHub, airport->logCount, sizeof(LogInfo), compare);
    FILE *send = fdopen(dup(client->socket), "w+");
    for (int i = 0; i < airport->logCount; ++i) {
        fprintf(send, "%s\n", airport->logHub[i].record);
        fflush(send);
    }
    fprintf(send, "%c\n", '.');
    fflush(send);
    fclose(send);
    sem_post(&airport->semaphore);                    // critical region
}

/**
 * Sends back this airport's info terminated by a newline
 * @param client: a pointer of client instance
 */
void send_airport_info(Client *client) {
    FILE *send = fdopen(dup(client->socket), "w+");
    fprintf(send, "%s\n", airport->info);
    fflush(send);
    fclose(send);
}

/**
 * Stores a plane's ID which has visited this airport into logHub,
 * and records total number of planes which have visited the airport
 * @param id: ID of a plane which visited this airport
 */
void log_plane_id(const char *id) {
    char *temp = malloc((strlen(id) + 1) * sizeof(char));
    assert(temp != NULL);
    memcpy(temp, id, (strlen(id) + 1) * sizeof(char));
    sem_wait(&airport->semaphore);                     // critical region
    ++(airport->logCount);
    airport->logHub = realloc(airport->logHub,
            airport->logCount * sizeof(LogInfo));
    assert(airport->logHub != NULL);
    airport->logHub[airport->logCount - 1].record = temp;
    sem_post(&airport->semaphore);                      // critical region
}

/**
 * Airport server acts on corresponding command
 * @param command: the received command massage
 * @param client: a pointer of client instance
 */
void execute_command(const char *command, Client *client) {
    if (strcmp(command, "logHub") == 0) {
        send_plane_logs(client);
    } else {
        log_plane_id(command);
        send_airport_info(client);
    }
}

/**
 * Acts on any command the server received via connection
 * if client disconnected, this server thread would close the socket,
 * terminate and release the memory automatically
 * @param argument: a pointer of client instance
 * @return NULL
 */
void *run_request(void *argument) {
    Client *client = (Client *)argument;
    while (1) {
        char *message = malloc(MAX_TEXT_LEN * sizeof(char));
        assert(message != NULL);
        memset(message, 0, MAX_TEXT_LEN * sizeof(char));
        ssize_t count = read_line(client->socket, message, MAX_TEXT_LEN);
        if (count <= 0) {
            free(message);
            break; // client has disconnected with server
        } else {
            execute_command(message, client);
            free(message);
        }
    }
    shutdown(client->socket, SHUT_WR);
    close(client->socket);
    free(client);
    return (void *)0;
}

/**
 * Connects to mapper server, registers this
 * airport server's ID and port number
 * @return AirportStatus
 */
AirportStatus connect_to_mapper(void) {
    struct sockaddr_in server;
    int airportSock, link;
    airportSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (airportSock < 0) {
        return BAD_CONN;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(airport->mapperPort);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    link = connect(airportSock, (struct sockaddr *)&server, sizeof(server));
    if (link < 0) {
        return BAD_CONN;
    }
    FILE *send = fdopen(dup(airportSock), "w+");
    fprintf(send, "!%s:%d\n", airport->name, airport->selfPort);
    fflush(send);
    shutdown(airportSock, SHUT_WR);
    fclose(send);
    close(airportSock);
    return A_OK;
}

/**
 * Starts a server side
 * via IPv4 TCP networking
 */
void start_server(void) {
    struct sockaddr_in server, self;
    socklen_t serverSize, clientSize;
    int listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock < 0) {
        perror("starting server failed");
        return;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = SYS_SPECIFIED_PORT;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenSock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server binding error");
        return;
    }
    listen(listenSock, MAX_QUEUE);
    serverSize = sizeof(self);
    getsockname(listenSock, (struct sockaddr *)&self, &serverSize);
    airport->selfPort = ntohs(self.sin_port);
    printf("%d\n", airport->selfPort);
    if (airport->mapperPort != 0) {         //register ID and port
        AirportStatus status = connect_to_mapper();
        if (status == BAD_CONN) {
            airport->connectFailed = true;
            return;
        }
    }
    while (1) {                 //server circularly runs until killed
        struct sockaddr_in client;
        clientSize = sizeof(client);
        int connectSock = accept(listenSock,
                (struct sockaddr *)&client, &clientSize);
        Client *guest = malloc(sizeof(Client));
        assert(guest != NULL);
        guest->socket = connectSock;
        pthread_t thread;
        pthread_create(&thread, NULL, run_request, guest);
        pthread_detach(thread);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        return show_message(BAD_NUM);
    }
    for (int i = 1; i < argc; ++i) {
        if (!is_valid_argument(argv[i])) {
            return show_message(BAD_CHAR);
        }
    }
    if (argc == 4 && !is_valid_port(argv[3])) {
        return show_message(BAD_PORT);
    }
    init_airport(argc, argv);
    start_server();
    if (airport->connectFailed) {
        return show_message(BAD_CONN);
    }
    free(airport);
    return show_message(A_OK);
}
