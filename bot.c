#include <stdlib.h>
#include "board.h"
#include "bot.h"

#define MAX_MOVES 100
extern char h_lines[5][5];
extern char v_lines[4][6];
extern char boxes[4][5];


int creates_third_side(Move m) { // check for 3 sides to make sure it takes points when possible
    int r1 = m.r1, c1 = m.c1, r2 = m.r2, c2 = m.c2;

    if (r1 == r2) { //horinzontal check
        int r = r1;
        int c = (c1 < c2) ? c1 : c2;

        
        if (r > 0) { // above completed?
            int sides = h_lines[r-1][c] + v_lines[r-1][c] + v_lines[r-1][c+1];
            if (sides == 2) return 1;
        }
        if (r < 4) { // below?
            int sides = h_lines[r+1][c] + v_lines[r][c] + v_lines[r][c+1];
            if (sides == 2) return 1;
        }

    } else { // vertical line might make a box?
        int r = (r1 < r2) ? r1 : r2;
        int c = c1;
        if (c > 0) { // left box ?
            int sides = v_lines[r][c-1] + h_lines[r][c-1] + h_lines[r+1][c-1];
            if (sides == 2) return 1;
        }
        if (c < 5) { // right box?
            int sides = v_lines[r][c+1] + h_lines[r][c] + h_lines[r+1][c];
            if (sides == 2) return 1;
        }
    }

    return 0;
}

Move get_bot_move() { // finds the valid moves gets a point if possible avoids 3 lines if possible if not randomize 
    Move moves[MAX_MOVES];
    int count = get_all_valid_moves(moves, MAX_MOVES);

    for (int i = 0; i < count; i++) { // if box do it
        Move m = moves[i];

        if (would_complete_box(m.r1, m.c1, m.r2, m.c2) > 0) {
            return m;
        }
    }
    for (int i = 0; i < count; i++) { // no box dont give a 3 liner
        if (!creates_third_side(moves[i])) {
            return moves[i];
        }
    }
    return moves[rand() % count]; // no choice random
}