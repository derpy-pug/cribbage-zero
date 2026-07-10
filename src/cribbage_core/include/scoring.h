#ifndef SCORING_H
#define SCORING_H

#include "card_pile.h"
#include "hand.h"

namespace cribbage {

int score_pile(const CardPile& pile);

int score_hand(const Hand& hand, Card cut, bool is_crib);

int score_crib(
    const Card& discard1,
    const Card& discard2,
    const Card& opponent1,
    const Card& opponent2,
    const Card& cut
);
int score_hand_suitless(const Hand& hand, Card cut);

int score_pair_count(int count);

}  // namespace cribbage
#endif  // SCORING_H
