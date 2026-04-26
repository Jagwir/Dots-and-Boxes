#include <stdio.h>
#include <string.h>
#include "game.h"
#include "network.h"

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "server") == 0) { // start as server
        int socket_fd = start_server(GAME_PORT);
        run_network_game(socket_fd, 'A');
        close_connection(socket_fd);
        return 0;
    }

    if (argc >= 3 && strcmp(argv[1], "client") == 0) { // start as player 2 basically
        int socket_fd = connect_to_server(argv[2], GAME_PORT);
        run_network_game(socket_fd, 'B');
        close_connection(socket_fd);
        return 0;
    }

    init_game(); 
    run_game(); // start regular vs bot or local against a player same machine
    return 0;
}