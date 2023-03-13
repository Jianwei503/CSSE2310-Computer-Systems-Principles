#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <netdb.h>

#define LOCALHOST "127.0.0.1"
#define MAX_TEXT_LEN 80

typedef struct {
    char *line;
} Line;

typedef struct {
    int lineCount;
    Line *lineHub;
    sem_t sem;
} Server;

typedef struct {
    int socket;
} Client;

typedef struct {
    Server *server;
    Client *client;
} Hub;

bool is_valid_arg(char **argv) {
    for (int i = 0; i < strlen(argv[1]); ++i) {
        if (!isdigit(argv[i])) {
            return false;
        }
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

/**
 * Acts on any command the mapper server received via connection
 * if client disconnected, this server thread would close the socket,
 * terminate and release the memory automatically
 * @param argument: a pointer of client instance
 * @return NULL
 */
void *run_request(void *argument) {
    Hub *hub = (Hub *)argument;
    while (1) {
        char *message = malloc(MAX_TEXT_LEN * sizeof(char));
        assert(message != NULL);
        memset(message, 0, MAX_TEXT_LEN * sizeof(char));
        ssize_t count;
        while ((count = read_line(hub->client->socket, message,
                MAX_TEXT_LEN)) > 0) {
            sem_wait(&hub->server->sem);
            ++(hub->server->lineCount);
            hub->server->lineHub = realloc(hub->server->lineHub,
                    hub->server->lineCount * sizeof(Line));
            hub->server->lineHub[hub->server->lineCount - 1].line = message;
            sem_post(&hub->server->sem);
            free(message);
        }
    shutdown(hub->client->socket, SHUT_WR);
    close(hub->client->socket);
    free(hub);
    return (void *)0;
}

/**
 * Starts a server side
 * via IPv4 TCP networking
 */
void start_server(void) {
    Server *server = malloc(sizeof(Server));
    server->lineCount = 0;
    server->lineHub = NULL;
    sem_init(&server->sem, 0, 1);
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
        return;
    }
    if (bind(listenSock, (struct sockaddr *) ai->ai_addr,
             sizeof(struct sockaddr)) < 0) {
        return;
    }
    struct sockaddr_in self;
    memset(&self, 0, sizeof(struct sockaddr_in));
    socklen_t length = sizeof(struct sockaddr_in);
    getsockname(listenSock, (struct sockaddr *) &self, &length);
    printf("%d\n", ntohs(self.sin_port));
    fflush(stdout);
    listen(listenSock, SOMAXCONN);
    int connectSock;
    while (connectSock = accept(listenSock, 0, 0), connectSock >= 0) {
        Client *client = malloc(sizeof(Client));
        client->socket = connectSock;
        Hub *hub = malloc(sizeof(Hub));
        hub->server = server;
        hub->client = client;
        pthread_t thread;
        pthread_create(&thread, NULL, run_request, hub);
        pthread_detach(thread);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        puts("Invalid arg numbers");
        return 0;
    }
    if (!is_valid_arg(argv)) {
        puts("Invalid number");
    }
    int num = atoi(argv[1]);
    for (int i = 0; i < num; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            start_server();
            return 0;
        }
    }
    wait(NULL);
    return 0;
}
}
