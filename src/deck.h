#ifndef DECK_H
#define DECK_H

#include <vector>

#include "card.h"
#include "card_container.h"
#include "hand.h"


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
     * @brief Shuffles the top cards of the deck into the deck
     * 
     * Used for performance, so there is less unnecessary shuffling.
     * Should have the same effect of shuffle(), but faster.
     *
     * @param numCards The number of cards on the top of the deck 
     * to shuffle into the whole deck
     */
    void shuffleTopCardsIntoDeck(int numCards);

    /*
     * @brief Deals a card from the deck
     *
     * The deck does not lose the card dealt,
     * so shuffling gets all the cards back.
     *
     * @return The card dealt from the deck
     */
    Card deal_card();

    /*
     * @brief Deals a hand from the deck
     * 
     * The deck does not lose the cards dealt,
     * so shuffling gets all the cards back.
     *
     * @param numCards The number of cards to deal
     * @return The hand dealt from the deck
     */
    Hand deal_hand(int numCards); 

private:
    int next_card_idx;
};


#endif // DECK_H
