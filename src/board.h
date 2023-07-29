#ifndef BOARD_H
#define BOARD_H

#include "player.h"

typedef struct {
    int front;
    int back;
} PegPositions;

typedef struct {
    PegPositions player1;
    PegPositions player2;
} Board;

void init_board(Board* board);

void move_peg(Board* board, Player player, int spaces);

char is_winner(const Board* board);

// Assumes that there is a winner
// Check if there is a winner before calling this function
Player get_winner(const Board* board);

int get_peg_position(const Board* board, Player player);

float get_probability_of_win(const Board* board, Player player);

void print_board(const Board* board, const PlayerInfo* player1, const PlayerInfo* player2);

#endif
