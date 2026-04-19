#ifndef BOARD_H
#define BOARD_H
typedef struct {
    int r1, c1, r2, c2;
} Move;
int would_complete_box(int r1, int c1, int r2, int c2);
void init_board();
void print_board();
int make_move(int r1, int c1, int r2, int c2, char player);
int is_game_over();
int get_all_valid_moves(Move moves[], int maxMoves);
#endif