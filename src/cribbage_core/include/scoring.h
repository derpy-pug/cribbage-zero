#ifndef SCORING_H
#define SCORING_H

#include "card_pile.h"
#include "hand.h"

int score_pile(const CardPile& pile);

int score_hand(const Hand& hand, Card cut, bool is_crib);

int score_hand_suitless(const Hand& hand, Card cut);

int score_pair_count(int count);

#endif  // SCORING_H
