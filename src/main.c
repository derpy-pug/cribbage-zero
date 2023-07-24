#include "cards.h"
#include "cribbage.h"
#include "cribbage_scoring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void play_games(int num_games, PlayerInfo *player1, PlayerInfo *player2) {
    Card deck[52];
    new_deck(deck);
    for (int i = 0; i < num_games; i++) {
        player1->score = 0;
        player2->score = 0;
        Winner winner = cribbage_game(deck, player1, player2);
        if (winner == DEALER) player1->wins++;
        else if (winner == PONE) player2->wins++;
        else printf("Tie\n");
        //printf("Winner is %s\n", winner == DEALER ? player1->name : player2->name);
        //printf("Final Score: %d - %d\n", player1->score, player2->score);
    }
}

char score_hands(int num_hands) {
    Card deck[52];
    new_deck(deck);
    shuffle_deck(deck);
    Hand hand = new_hand(deck, 4);
    int score;
    for (int i = 0; i < num_hands; i++) {
        score = score_hand(hand, deck[5], i%2);
        /*
        sort_hand(&hand);
        print_hand(hand);
        printf("Cut: ");
        print_card(deck[5]);
        printf("Score: %d\n", score);
        */
        shuffle_used_deck(deck, 5);
    }
    return score;
}

int main() {
    random_init();
    init_scoring(); 

    PlayerInfo player1 = {PLAYER1, 0, RANDOM, "Derpy Pug", 0, 0};
    PlayerInfo player2 = {PLAYER2, 0, RANDOM, "Randy", 0, 0};
    
    play_games(10000, &player1, &player2);
    /*
    play_games(100000, &player1, &player2);
    printf("Total Wins: %d - %d\n", player1.wins, player2.wins);
    */
    free_scoring();  
    return 0;
}
