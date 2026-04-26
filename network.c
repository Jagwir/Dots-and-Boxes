#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network.h"

int start_server(int port) { // this is the server side
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // this opens the tcp socket 
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { // this allows socket reuse for quick restarting of the server 
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // attach socket to chosen port
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) { // this listens for a connection  
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client on port %d...\n", port); // keeps listening til the client side or player B connects 
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");
    close(server_fd);
    return client_fd;
}

int connect_to_server(const char *ip, int port) { // this is the client side helps it connect to the server aka player A it works by sending its moves and not the whole board 
    int socket_fd;
    struct sockaddr_in server_address;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0) {
        perror("invalid server IP address");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connecting to %s:%d...\n", ip, port);
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");
    return socket_fd;
}

int send_move(int socket_fd, int r1, int c1, int r2, int c2) { // sends the move to player A
    char buffer[64];
    int length = snprintf(buffer, sizeof(buffer), "%d %d %d %d\n", r1, c1, r2, c2);

    if (send(socket_fd, buffer, length, 0) != length) {
        perror("send failed");
        return -1;
    }

    return 0;
}

int receive_move(int socket_fd, int *r1, int *c1, int *r2, int *c2) { //receives the move from player A 
    char buffer[64];
    int index = 0;
    char ch;

    while (index < (int)sizeof(buffer) - 1) {
        ssize_t bytes = recv(socket_fd, &ch, 1, 0);
        if (bytes <= 0) {
            return -1;
        }

        if (ch == '\n') {
            break;
        }

        buffer[index++] = ch;
    }

    buffer[index] = '\0';

    if (sscanf(buffer, "%d %d %d %d", r1, c1, r2, c2) != 4) {
        return -1;
    }

    return 0;
}

void close_connection(int socket_fd) {
    close(socket_fd);
}
