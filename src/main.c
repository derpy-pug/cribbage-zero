#include "cards.h"
#include "cribbage.h"
#include "gen_stats.h"
#include "utils/random.h"
#include "cribbage_scoring.h"
#include "discard.h"
#include "discard_table.h"
#include <stdio.h>
#include <stdlib.h>

void play_games(int num_games, PlayerInfo *player1, PlayerInfo *player2) {
    Card deck[52];
    init_deck(deck);
    for (int i = 0; i < num_games; i++) {
        if (i % 1000 == 0) printf("Game %d\n", i);
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
    init_deck(deck);
    shuffle_deck(deck);
    Hand hand = init_hand(deck, 4);
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

void print_hand_format(Hand hand, char is_my_crib) {
    for (int i = 0; i < hand.length; i++) {
        print_card(hand.cards[i]);
    }   
    printf("%c", is_my_crib ? 'Y' : 'N');
    printf("\n");
}

char score_hands_discard_stats(int num_hands) {
    Card deck[52];
    init_deck(deck);
    shuffle_deck(deck);
    Hand hand = init_hand(deck, 6);
    for (int i = 0; i < num_hands; i++) {
        sort_hand(&hand);
        char is_my_crib = i%2;
        DiscardStats* sorted_discards = sorted_discard_stats(hand, is_my_crib);
        print_hand_format(hand, is_my_crib);
        printf("Hand: ");
        print_hand(hand);
        printf("\n");
        if (is_my_crib) printf("My Crib\n");
        else printf("Opp Crib\n");
        print_all_discard_stats(sorted_discards, hand, 3);
        free(sorted_discards);
        printf("\n");
        shuffle_used_deck(deck, 5);
    }
    return 0;
}

int main(int argc, char** argv) {
    random_init();
    init_scoring(); 
    //check_scoring();

    //load_mean_discard_tables("txt/discard_table_AI_AI_v1.0.txt");
    //generate_min_max_crib_discard_tables();
    /*
    Stats stats = {0};
    Stats flush_stats = {0};
    int hand[5] = {2, 11, 5, 5, 5};
    min_max_stats(hand, &stats, &flush_stats);
    printf("Max: %d\n", stats.max);
    printf("Max: %d\n", flush_stats.max);
    */

    //score_hands_discard_stats(10);

    //print_cribbage_stats();

    PlayerInfo player1 = {PLAYER1, AI, "Derpy Pug", 0, 0};
    PlayerInfo player2 = {PLAYER2, RANDOM, "Randy", 0, 0};

    //print_discard_tables();
   
    play_games(10000, &player1, &player2);
    printf("Total Wins: %d - %d\n", player1.wins, player2.wins);
    /*
    */
    free_scoring();  
    return 0;
}
