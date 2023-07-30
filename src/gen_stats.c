#include "gen_stats.h"
#include "cribbage_core.h"
#include "player.h"
#include <stdio.h>

/*
 * PLAY_AI_BASIC
 * Mean dealer score: 15.964406 +- 0.017234
 * Mean pone score: 7.332087 +- 0.0099950
*/ 


void print_cribbage_stats() {
    Card deck[52];
    init_deck(deck);
    shuffle_deck(deck);
    Board board;
    PlayerInfo player1 = {PLAYER1, AI, "Derpy Pug", 0, 0};
    PlayerInfo player2 = {PLAYER2, RANDOM, "Randy", 0, 0};

    long long dealer_score = 0;
    long long pone_score = 0;
    const int NUM_GAMES = 100000;
    int i = NUM_GAMES;
    while(i--) {
    init_board(&board);
    cribbage_round(deck, &player1, &player2, &board); 

    dealer_score += board.player1.front;
    pone_score += board.player2.front;
    shuffle_used_deck(deck, 13);
    }

    double avg_dealer_score = (double)dealer_score / NUM_GAMES;
    double avg_pone_score = (double)pone_score / NUM_GAMES;

    printf("Dealer average score: %f\n", avg_dealer_score);
    printf("Pone average score: %f\n", avg_pone_score);

}
