#include <stdio.h>
#include <pthread.h>
#include "game.h"
#include "board.h"
#include "bot.h"
#include "network.h"

char currentPlayer = 'A'; //current game state
int scoreA = 0, scoreB = 0;

static pthread_mutex_t board_lock = PTHREAD_MUTEX_INITIALIZER; //protect the board 
static pthread_mutex_t inbox_lock = PTHREAD_MUTEX_INITIALIZER; // this will help communication between the threads 
static pthread_cond_t inbox_ready = PTHREAD_COND_INITIALIZER;

static Move received_move;
static int has_received_move = 0;
static int network_disconnected = 0; //flag to solve disconnect issues so that you are not just stuck waiting on the disconnected player

typedef struct {
    int socket_fd;
} ReceiverArgs;

void switch_player() {
    currentPlayer = (currentPlayer == 'A') ? 'B' : 'A';
}
void init_game() {
    currentPlayer = 'A';
    scoreA = 0;
    scoreB = 0;
    init_board();
}
static void clear_input_line() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
static void apply_score(char player, int completed_boxes) { // score updater 
    if (completed_boxes <= 0) {
        return;
    }

    if (player == 'A') {
        scoreA += completed_boxes;
    } else {
        scoreB += completed_boxes;
    }
}

void run_game() { this will run the game against a bot or local 2 players
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
            clear_input_line();
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
            apply_score(currentPlayer, result);
        }
    }
    print_board();
    printf("\nFINAL SCORE A:%d B:%d\n", scoreA, scoreB);
    if (scoreA > scoreB) printf("A wins!\n");
    else if (scoreB > scoreA) printf("B wins!\n");
    else printf("Draw!\n");
}

static void *receive_thread(void *arg) { // tjis thread runs to receive the moves by player 2
    ReceiverArgs *receiver_args = (ReceiverArgs *)arg;
    while (!network_disconnected) {
        Move move;
        if (receive_move(receiver_args->socket_fd, &move.r1, &move.c1, &move.r2, &move.c2) != 0) {
            pthread_mutex_lock(&inbox_lock);
            network_disconnected = 1;
            pthread_cond_signal(&inbox_ready);
            pthread_mutex_unlock(&inbox_lock);
            break;
        }
        pthread_mutex_lock(&inbox_lock);
        while (has_received_move && !network_disconnected) {
            pthread_cond_wait(&inbox_ready, &inbox_lock);
        }
        received_move = move;
        has_received_move = 1;
        pthread_cond_signal(&inbox_ready);
        pthread_mutex_unlock(&inbox_lock);
    }
    return NULL;
}

static int wait_for_opponent_move(Move *move) { // this is the block that makes sure you cannot act if it is not your turn 
    pthread_mutex_lock(&inbox_lock);
    while (!has_received_move && !network_disconnected) {
        pthread_cond_wait(&inbox_ready, &inbox_lock);
    }
    if (network_disconnected) {
        pthread_mutex_unlock(&inbox_lock);
        return -1;
    }
    *move = received_move;
    has_received_move = 0;
    pthread_cond_signal(&inbox_ready);
    pthread_mutex_unlock(&inbox_lock);
    return 0;
}
void run_network_game(int socket_fd, char myPlayer) { // this is the "online" feature to ruyn 2 machines same game 
    pthread_t receiver_thread_id;
    ReceiverArgs receiver_args;
    char opponent = (myPlayer == 'A') ? 'B' : 'A';

    init_game();
    has_received_move = 0;
    network_disconnected = 0;
    receiver_args.socket_fd = socket_fd;
    if (pthread_create(&receiver_thread_id, NULL, receive_thread, &receiver_args) != 0) {
        printf("Could not start receiver thread.\n");
        return;
    }
    printf("Network game started. You are Player %c.\n", myPlayer);
    printf("Player A starts.\n");

    while (!is_game_over() && !network_disconnected) {
        pthread_mutex_lock(&board_lock);
        print_board();
        printf("\nScore A: %d | Score B: %d\n", scoreA, scoreB);
        pthread_mutex_unlock(&board_lock);

        if (currentPlayer == myPlayer) {
            int r1, c1, r2, c2;
            int result;

            printf("Your turn. Enter r1 c1 r2 c2: ");
            if (scanf("%d %d %d %d", &r1, &c1, &r2, &c2) != 4) {
                printf("Invalid input!\n");
                clear_input_line();
                continue;
            }
            pthread_mutex_lock(&board_lock);
            result = make_move(r1, c1, r2, c2, myPlayer);
            if (result == -1) {
                pthread_mutex_unlock(&board_lock);
                printf("Invalid move! Try again.\n");
                continue;
            }
            apply_score(myPlayer, result);
            if (result == 0) {
                switch_player();
            }
            pthread_mutex_unlock(&board_lock);

            if (send_move(socket_fd, r1, c1, r2, c2) != 0) {
                printf("Connection lost while sending move.\n");
                network_disconnected = 1;
                break;
            }
        } else { //  this is the block waiting for move from the other player
            Move move;
            int result;

            printf("Waiting for Player %c move...\n", opponent);
            if (wait_for_opponent_move(&move) != 0) {
                printf("Connection lost while waiting for opponent.\n");
                break;
            }

            pthread_mutex_lock(&board_lock);
            result = make_move(move.r1, move.c1, move.r2, move.c2, opponent);
            if (result == -1) {
                pthread_mutex_unlock(&board_lock);
                printf("Received invalid move from opponent. Ending game.\n");
                network_disconnected = 1;
                break;
            }
            printf("Player %c played: %d %d %d %d\n", opponent, move.r1, move.c1, move.r2, move.c2);
            apply_score(opponent, result);
            if (result == 0) {
                switch_player();
            }
            pthread_mutex_unlock(&board_lock);
        }
    }

    pthread_mutex_lock(&board_lock); // final board and result printing
    print_board();
    printf("\nFINAL SCORE A:%d B:%d\n", scoreA, scoreB);
    if (scoreA > scoreB) printf("A wins!\n");
    else if (scoreB > scoreA) printf("B wins!\n");
    else printf("Draw!\n");
    pthread_mutex_unlock(&board_lock);

    network_disconnected = 1;
    pthread_cancel(receiver_thread_id);
    pthread_join(receiver_thread_id, NULL);
}
