#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#define ROWS 5
#define COLS 6

char h_lines[ROWS][COLS - 1];
char v_lines[ROWS - 1][COLS];
char boxes[ROWS - 1][COLS - 1];


void init_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS - 1; j++) {
            h_lines[i][j] = 0;
        }
    }
    for (int i = 0; i < ROWS - 1; i++) {
        for (int j = 0; j < COLS; j++) {
            v_lines[i][j] = 0;
        }
    }
    for (int i = 0; i < ROWS - 1; i++) {
        for (int j = 0; j < COLS - 1; j++) {
            boxes[i][j] = ' ';
        }
    }
}




void print_board() {
    for (int i = 0; i < ROWS; i++) {

        // prints the dots and horizontal lines
        for (int j = 0; j < COLS; j++) {
            printf(".");

            if (j < COLS - 1) {
                if (h_lines[i][j])
                    printf("---");
                else
                    printf("   ");
            }
        }
        printf("\n");
        // this is for vert lines and claimed boxes
        if (i < ROWS - 1) {
            for (int j = 0; j < COLS; j++) {

                if (v_lines[i][j])
                    printf("|");
                else
                    printf(" ");

                if (j < COLS - 1) {
                    printf(" %c ", boxes[i][j]);
                }
            }
            printf("\n");
        }
    }
}

int is_valid_move(int r1, int c1, int r2, int c2) {
    //first bug fix adds borders making sure you can go past the play field example line for 6 0 7 0
    if (r1 < 0 || r1 >= ROWS ||
        r2 < 0 || r2 >= ROWS ||
        c1 < 0 || c1 >= COLS ||
        c2 < 0 || c2 >= COLS) {
        return 0;
    }
    // check for touching 
    if (!((abs(r1 - r2) == 1 && c1 == c2) ||
          (abs(c1 - c2) == 1 && r1 == r2)))
        return 0;
    if (r1 == r2) { //  the fix to make sure the same move cant be duped
        int c = (c1 < c2) ? c1 : c2;

        if (h_lines[r1][c] == 1)
            return 0; 
    } else {
        int r = (r1 < r2) ? r1 : r2;

        if (v_lines[r][c1] == 1)
            return 0; 
    }
    return 1;
}



void draw_line(int r1, int c1, int r2, int c2) {
    if (r1 == r2) {
        int c = (c1 < c2) ? c1 : c2;
        h_lines[r1][c] = 1;
    } else {
        int r = (r1 < r2) ? r1 : r2;
        v_lines[r][c1] = 1;
    }
}



int check_box(int r, int c, char player) {
    if (boxes[r][c] != ' ') return 0;

    if (h_lines[r][c] &&
        h_lines[r + 1][c] &&
        v_lines[r][c] &&
        v_lines[r][c + 1]) {

        boxes[r][c] = player;
        return 1;
    }
    return 0;
}
int make_move(int r1, int c1, int r2, int c2, char player) {

    if (!is_valid_move(r1, c1, r2, c2))
        return -1;
    draw_line(r1, c1, r2, c2);
    int completed = 0;

    // check affected boxes
    if (r1 == r2) {
        int r = r1;
        int c = (c1 < c2) ? c1 : c2;

        if (r > 0) completed += check_box(r - 1, c, player);
        if (r < ROWS - 1) completed += check_box(r, c, player);
    } else {
        int r = (r1 < r2) ? r1 : r2;
        int c = c1;
        if (c > 0) completed += check_box(r, c - 1, player);
        if (c < COLS - 1) completed += check_box(r, c, player);
    }
    return completed;
}
int is_game_over() {
    for (int i = 0; i < ROWS - 1; i++)
        for (int j = 0; j < COLS - 1; j++)
            if (boxes[i][j] == ' ')
                return 0;
    return 1;
}

int get_all_valid_moves(Move moves[], int maxMoves) { //finds all the valid moves, counts them and adds a move struct for each valid move so that the bot knows its options
    int count = 0;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (c + 1 < COLS) { // going right
                if (is_valid_move(r, c, r, c + 1)) {
                    if (count < maxMoves) {
                        moves[count++] = (Move){r, c, r, c + 1};
                    }
                }
            }
            if (r + 1 < ROWS) { // going down
                if (is_valid_move(r, c, r + 1, c)) {
                    if (count < maxMoves) {
                        moves[count++] = (Move){r, c, r + 1, c};
                    }
                }
            }
        }
    }

    return count;
}