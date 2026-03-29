#include <stdio.h>
#include "game.h"
#include "board.h"

char currentPlayer = 'A';
int scoreA = 0, scoreB = 0;

void switch_player() {
    currentPlayer = (currentPlayer == 'A') ? 'B' : 'A';
}

void init_game() {
    init_board();
}

void run_game() {
    while (!is_game_over()) {

        print_board();
        printf("\nScore A: %d | Score B: %d\n", scoreA, scoreB);
        printf("Player %c turn\n", currentPlayer);
        int r1, c1, r2, c2;
        printf("Enter r1 c1 r2 c2: ");
        if (scanf("%d %d %d %d", &r1, &c1, &r2, &c2) != 4) {
            printf("Invalid move!\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }
        int result = make_move(r1, c1, r2, c2, currentPlayer);

        if (result == -1) {
            printf("Invalid move!\n");
            continue;
        }

        if (result == 0) {
            switch_player();
        } else {
            if (currentPlayer == 'A') scoreA += result;
            else scoreB += result;
        }
    }
    print_board();
    printf("\nFINAL SCORE A:%d B:%d\n", scoreA, scoreB);
    if (scoreA > scoreB) printf("A wins!\n");
    else if (scoreB > scoreA) printf("B wins!\n");
    else printf("Draw!\n");
}