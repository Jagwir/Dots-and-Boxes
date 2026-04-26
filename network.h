#ifndef NETWORK_H
#define NETWORK_H

#define GAME_PORT 8080 

int start_server(int port);
int connect_to_server(const char *ip, int port);
int send_move(int socket_fd, int r1, int c1, int r2, int c2);
int receive_move(int socket_fd, int *r1, int *c1, int *r2, int *c2);
void close_connection(int socket_fd);

#endif