#ifndef DECK_H
#define DECK_H

#include <vector>

#include "card.h"
#include "card_container.h"


/*
 * @brief A deck of cards
 */
class Deck : public CardContainer
{
public:
    /*
     * @brief Constructs a deck of 52 cards
     */
    Deck();

    /*
     * @brief Makes a deck of 52 cards
     */
    void make_deck();

    /*
     * @brief Shuffles the deck
     */
    void shuffle();

    /*
     * @brief Deals a card from the deck
     *
     * @return The card dealt from the deck
     */
    Card deal_card();

private:
    std::vector<Card> cards;
};


#endif // DECK_H
