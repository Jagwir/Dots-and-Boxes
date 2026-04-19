#include <stdio.h>
#include "game.h"
#include "board.h"
#include "bot.h"

char currentPlayer = 'A';
int scoreA = 0, scoreB = 0;

void switch_player() {
    currentPlayer = (currentPlayer == 'A') ? 'B' : 'A';
}

void init_game() {
    init_board();
}

void run_game() {
    int vsBot;
    printf("Play vs bot? (1=yes 0=no): ");
    scanf("%d", &vsBot);
    while (!is_game_over()) {

        print_board();
        printf("\nScore A: %d | Score B: %d\n", scoreA, scoreB);
        printf("Player %c turn\n", currentPlayer);

        if (vsBot && currentPlayer == 'B') {
        Move m = get_bot_move();
        printf("Bot plays: %d %d %d %d\n", m.r1, m.c1, m.r2, m.c2);
        int result = make_move(m.r1, m.c1, m.r2, m.c2, currentPlayer);
        if (result == 0)
            switch_player();
        else
            scoreB += result;

        continue;
        }


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