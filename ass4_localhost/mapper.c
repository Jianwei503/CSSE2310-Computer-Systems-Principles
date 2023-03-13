#include "mapper.h"

/**
 * Compares two instances of struct
 * @param first: the first instance of struct that will be compared
 * @param second: the second instance of struct that will be compared
 * @return 0 if 1st instance's string == 2nd instance's string
 *         1 if 1st instance's string > 2nd instance's string
 *         -1 if 1st instance's string < 2nd instance's string
 */
int compare(const void *first, const void *second) {
    Airport *one = (Airport *)first;
    Airport *two = (Airport *)second;
    return strcmp(one->name, two->name);
}

/**
 * Prints the mappings to client, in lexicographic order of ID,
 * each entry ends with newline, ID and port number are separated
 * by a colon
 * @param client: a pointer of client instance
 */
void list_mapping(Client *client) {
    sem_wait(&mapper->semaphore);                     // critical region
    qsort(mapper->airHub, mapper->airCount, sizeof(Airport), compare);
    FILE *send = fdopen(dup(client->socket), "w+");
    for (int i = 0; i < mapper->airCount; ++i) {
        fprintf(send, "%s:%d\n",
                mapper->airHub[i].name, mapper->airHub[i].selfPort);
        fflush(send);
    }
    fclose(send);
    sem_post(&mapper->semaphore);                     // critical region
}

/**
 * Sends back the port number followed by a newline to client
 * if there is an entry corresponding to the given ID, or
 * sends back a ';' followed by a newline if there is no mapping
 * @param command: the received command massage, contains ID
 * @param client: a pointer of client instance
 */
void query_mapping(const char *command, Client *client) {
    sem_wait(&mapper->semaphore);                     // critical region
    FILE *send = fdopen(dup(client->socket), "w+");
    for (int i = 0; i < mapper->airCount; ++i) {
        if (strcmp(mapper->airHub[i].name, command + strlen("?")) == 0) {
            fprintf(send, "%d\n", mapper->airHub[i].selfPort);
            fflush(send);
            fclose(send);
            return;
        }
    }
    fprintf(send, "%c\n", ';'); // no mapping id
    fflush(send);
    fclose(send);
    sem_post(&mapper->semaphore);                     // critical region
}

/**
 * Adds airport's ID and port number as an entry into mapper's log
 * @param command: the received command massage, contains ID and port
 */
void update_mapping(const char *command) {
    char temp[MAX_TEXT_LEN] = "";
    strcpy(temp, command + strlen("!"));
    char *separator = ":";
    char *tempId = strtok(temp, separator);
    char *tempPort = strtok(NULL, separator);
    int port = atoi(tempPort);
    char *id = malloc((strlen(tempId) + 1) * sizeof(char));
    assert(id != NULL);
    memcpy(id, tempId, (strlen(tempId) + 1) * sizeof(char));
    sem_wait(&mapper->semaphore);                     // critical region
    for (int i = 0; i < mapper->airCount; ++i) {
        if (strcmp(mapper->airHub[i].name, id) == 0) { // ignore existed ID
            free(id);
            return;
        }
    }
    ++(mapper->airCount);
    mapper->airHub = realloc(mapper->airHub,
            mapper->airCount * sizeof(Airport));
    assert(mapper->airHub != NULL);
    mapper->airHub[mapper->airCount - 1].name = id;
    mapper->airHub[mapper->airCount - 1].selfPort = port;
    sem_post(&mapper->semaphore);                     // critical region
}

/**
 * Mapper server acts on corresponding command
 * @param command: the received command massage
 * @param client: a pointer of client instance
 */
void execute_command(const char *command, Client *client) {
    switch (analyze_command(command)) {
        case QUERY:
            query_mapping(command, client);
            break;
        case ADD:
            update_mapping(command);
            break;
        case LIST:
            list_mapping(client);
            break;
        default:
            break;
    }
}

/**
 * Determines which command the received massage is
 * @param command: the received command massage
 * @return QUERY if command starts with ? and is valid
 *         ADD if command starts with ! and is valid
 *         LIST if command starts with @ and is valid
 *         ERR if command is invalid
 */
Command analyze_command(const char *command) {
    int commandSize = (int)strlen(command) + 1; // 1 char for '\0'
    char *message = malloc(commandSize * sizeof(char));
    assert(message != NULL);
    memset(message, 0, commandSize * sizeof(char));
    memcpy(message, command, commandSize);
    if (strncmp(message, "?", strlen("?")) == 0 &&
            is_valid_argument(message + strlen("?"))) {
        free(message);
        return QUERY;
    } else if (strncmp(message, "!", strlen("!")) == 0) {
        char temp[MAX_TEXT_LEN] = "";
        strcpy(temp, message + strlen("!"));
        char *separator = ":";
        char *id = strtok(temp, separator);
        char *port = strtok(NULL, separator);
        char *end = strtok(NULL, separator);
        if (end == NULL &&
                is_valid_argument(id) &&
                is_valid_port(port)) {
            free(message);
            return ADD;
        } else {
            free(message);
            return ERR;
        }
    } else if (strcmp(message, "@") == 0) {
        free(message);
        return LIST;
    } else {
        free(message);
        return ERR;
    }
}

/**
 * Acts on any command the mapper server received via connection
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
 * Starts a server side
 * via IPv4 TCP networking
 */
void start_server(void) {
    struct addrinfo *ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int error = getaddrinfo(LOCALHOST, 0, &hints, &ai);
    if (error != 0) {
        freeaddrinfo(ai);
        return;
    }
    int listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock < 0) {
        perror("starting server failed");
        return;
    }
    if (bind(listenSock, (struct sockaddr *)ai->ai_addr,
            sizeof(struct sockaddr)) < 0) {
        perror("server binding error");
        return;
    }
    struct sockaddr_in self;
    memset(&self, 0, sizeof(struct sockaddr_in));
    socklen_t length = sizeof(struct sockaddr_in);
    getsockname(listenSock, (struct sockaddr*)&self, &length);
    printf("%d\n", ntohs(self.sin_port));
    fflush(stdout);
    listen(listenSock, SOMAXCONN);
    int connectSock;
    while (connectSock = accept(listenSock, 0, 0), connectSock >= 0) {
        //server circularly runs until killed
        Client *guest = malloc(sizeof(Client));
        assert(guest != NULL);
        guest->socket = connectSock;
        pthread_t thread;
        pthread_create(&thread, NULL, run_request, guest);
        pthread_detach(thread);
    }
}

/**
 * Initializes the instance of struct Mapper
 * holding by mapper server
 */
void init_mapper(void) {
    mapper = malloc(sizeof(Mapper));
    assert(mapper != NULL);
    mapper->airHub = NULL;
    mapper->airCount = 0;
    sem_init(&mapper->semaphore, 0, 1);
}

int main(void) {
    init_mapper();
    start_server();
    free(mapper);
    return 0;
}