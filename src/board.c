#include "board.h"
#include <stdio.h>


void init_board(Board* board) {
    board->player1 = (PegPositions){0, 0};
    board->player2 = (PegPositions){0, 0};
}

void move_peg(Board* board, Player player, int spaces) {
    PegPositions* peg_positions;
    switch (player) {
        case PLAYER1:
            peg_positions = &board->player1;
            break;
        case PLAYER2:
            peg_positions = &board->player2;
            break;
        default:
            printf("Invalid player to move peg\n");
            return;
    }

    peg_positions->back = peg_positions->front;
    peg_positions->front += spaces;
    if (peg_positions->front > 121) peg_positions->front = 121;
}

char is_winner(const Board* board) {
    if (board->player1.front == 121 || board->player2.front == 121) return 1;
    return 0;
}

// Assumes that there is a winner
// Check if there is a winner before calling this function
Player get_winner(const Board* board) {
    if (board->player1.front == 121) return PLAYER1;
    return PLAYER2;
}

int get_peg_position(const Board* board, Player player) {
    switch (player) {
        case PLAYER1:
            return board->player1.front;
        case PLAYER2:
            return board->player2.front;
        default:
            printf("Invalid player to get peg position\n");
            return -1;
    }
}

// TODO
float get_probability_of_win(const Board* board, Player player) {
    return 0.0;
}

void print_board(const Board* board, const PlayerInfo* player1, const PlayerInfo* player2) {
    printf("%s's Score: %d  -  %s's Score: %d\n", player1->name, board->player1.front, player2->name, board->player2.front);
}
