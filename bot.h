#ifndef BOT_H
#define BOT_H

#include "board.h"
int would_complete_box(int r1, int c1, int r2, int c2);
int get_all_valid_moves(Move moves[], int maxMoves);
Move get_bot_move();

#endif