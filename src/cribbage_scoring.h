#ifndef CRIBBAGE_SCORING_H
#define CRIBBAGE_SCORING_H

#include "cards.h"

char score_pile(const CardPile pile); 

char score_hand(const Hand hand, const Card cut, char is_crib);

/*
 * Initialize the hash table for scoring 15s, runs, and pairs.
 * This function must be called before calling score_hand.
*/ 
void init_scoring();

void free_scoring();
#endif
