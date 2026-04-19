#include <stdlib.h>
#include "board.h"
#include "bot.h"

#define MAX_MOVES 100
extern char h_lines[5][5];
extern char v_lines[4][6];
extern char boxes[4][5];

int creates_third_side(Move m) { //simulates moves to see if it creates a 3 line for the opponent if yes avoid the move if makes a full box take the point if neither random move
    int r1 = m.r1, c1 = m.c1, r2 = m.r2, c2 = m.c2;

    int count = 0;

    if (r1 == r2) { // box checkers left right up down
        int r = r1;
        int c = (c1 < c2) ? c1 : c2;

        if (r > 0) {
            int sides = h_lines[r-1][c] + v_lines[r-1][c] + v_lines[r-1][c+1];
            if (sides == 2) return 1;
        }
        if (r < 4) {
            int sides = h_lines[r+1][c] + v_lines[r][c] + v_lines[r][c+1];
            if (sides == 2) return 1;
        }
    } else {
        int r = (r1 < r2) ? r1 : r2;
        int c = c1;
        if (c > 0) {
            int sides = v_lines[r][c-1] + h_lines[r][c-1] + h_lines[r+1][c-1];
            if (sides == 2) return 1;
        }
        if (c < 5) {
            int sides = v_lines[r][c+1] + h_lines[r][c] + h_lines[r+1][c];
            if (sides == 2) return 1;
        }
    }
    return 0;
}



Move get_bot_move() {
    Move moves[MAX_MOVES];
    int count = get_all_valid_moves(moves, MAX_MOVES);
    for (int i = 0; i < count; i++) { //this will take a box if a box is possible 
        Move m = moves[i];
        int result = make_move(m.r1, m.c1, m.r2, m.c2, 'B');

        if (result > 0) {//undo move
            if (m.r1 == m.r2)
                h_lines[m.r1][(m.c1 < m.c2 ? m.c1 : m.c2)] = 0;
            else
                v_lines[(m.r1 < m.r2 ? m.r1 : m.r2)][m.c1] = 0;
            return m;
        }
        if (m.r1 == m.r2) //this will undo a move simulated by the bot   
            h_lines[m.r1][(m.c1 < m.c2 ? m.c1 : m.c2)] = 0;
        else
            v_lines[(m.r1 < m.r2 ? m.r1 : m.r2)][m.c1] = 0;
    }

    for (int i = 0; i < count; i++) { //will not create a 3 line for the opponent if possible
        if (!creates_third_side(moves[i])) {
            return moves[i];
        }
    }

    return moves[rand() % count]; //if no box is doable, and no move will create a 3 line, it resorts to a random move
}