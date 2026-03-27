#ifndef BOARD_H
#define BOARD_H

void init_board();
void print_board();
int make_move(int r1, int c1, int r2, int c2, char player);
int is_game_over();

#endif