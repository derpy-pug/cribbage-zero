#ifndef CRIBBAGE_SCORING_H
#define CRIBBAGE_SCORING_H

#include "cards.h"

char score_pile(const CardPile pile); 

char score_hand(const Hand hand, const Card cut, char is_crib);

// Must be 5 cards
char score_hand_suitless(int* cards);

char score_15(const int ranks[5]);

char score_flush(const Hand hand, const Card cut, char is_crib);

char score_hand_15s_runs_pairs(int hand[5]);

/*
 * Initialize the hash table for scoring 15s, runs, and pairs.
 * This function must be called before calling score_hand.
*/ 
void init_scoring();

void check_scoring();

void free_scoring();
#endif
