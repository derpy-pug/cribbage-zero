#ifndef CARD_PILE_H
#define CARD_PILE_H

#include <string>
#include <vector>

#include "card.h"

namespace cribbage {

class CardPile {
  public:
    CardPile();

    /*
     * @brief add a card to the pile
     *
     * @param card The card to add
     *
     * If can_add_card(card) is false, then next_round() is called, and
     * then the card is added.
     *
     */
    void add_card(Card card);

    /*
     * @brief check if the pile can add the given card
     *
     * A pile can add a card if the sum of the cards in the pile is <= 31
     *
     * @param card The card to check
     *
     * @return true if the pile can add the given card
     *        false otherwise
     */
    bool can_add_card(Card card) const;

    /*
     * @brief score the pile
     *
     * Scoring is only done on the current round of the pile
     * Scoring is done as follows:
     * - 15: If the sum of the cards is 15 then 2 points
     * - 31: If the sum of the cards is 31 then 1 point
     * - Pairs: If the last cards are the same rank then score pairs
     * - Runs: If the last cards are in a run then score runs
     *
     * @return The score of the pile
     */
    int score_pile() const;

    /*
     * @see score_pile()
     *
     * @brief score the pile with a possible additional card
     *
     * @param another_card The card to add to the pile
     *
     * @return The score of the pile with the additional card
     */
    int score_pile(Card another_card);


    /*
     * @brief Gets the string representation of the pile
     *
     * Example: "AS 10D KH QS | 5C 5H 5S"
     */
    std::string to_string() const;

    friend std::ostream& operator<<(std::ostream& os, const CardPile& pile);

  private:
    /*
     * @brief move to the next round of the pile
     *
     * Resets the current sum and updates the round offset
     */
    void next_round();

    int score_pile_run() const;

  private:
    int current_sum;
    std::vector<Card> cards;
    int current_round_offset;
    int previous_round_offset;
};

}  // namespace cribbage

#endif  // CARD_PILE_H
