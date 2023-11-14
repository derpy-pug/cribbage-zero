#ifndef DECK_H
#define DECK_H

#include <vector>

#include "card.h"
#include "card_container.h"
#include "hand.h"

namespace cribbage {

/*
 * @brief A deck of cards
 */
class Deck : public CardContainer {
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
    void shuffleTopCardsIntoDeck();

    /*
     * @brief Deals a card from the deck
     *
     * The deck does not lose the card dealt,
     * so shuffling gets this card back.
     *
     * @return The card dealt from the deck
     */
    Card deal_card();

    /*
     * @brief Deals a card from the deck and removes it from the deck
     *
     * The deck loses the card dealt,
     * so shuffling does not get this card back.
     * This is the same as deal_card(), but the card is removed from the deck.
     * To get the card back, the deck would need to be remade.
     *
     * @see deal_hand_and_remove()
     *
     * @return The card dealt from the deck
     */
    Card deal_card_and_remove();

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

    /*
     * @brief Deals a hand from the deck and removes it from the deck
     *
     * The deck loses the cards dealt,
     * so shuffling does not get these cards back.
     * This is the same as deal_hand(), but the cards are removed from the deck.
     * To get the cards back, the deck would need to be remade.
     *
     * Possible use case: dealing one hand to one player with `deal_hand_and_remove()`
     * and dealing the other hand to the other player with `deal_hand()`. Then,
     * shuffling the deck will get the cards back for the second player, but not
     * the first player. Then, the deck can be shuffled again but only for the 
     * second player. 
     * This would be faster than remaking/shuffling the deck every time and
     * removing the first hand from the deck.
     *
     * @param numCards The number of cards to deal
     * @return The hand dealt from the deck
     */
    Hand deal_hand_and_remove(int numCards);

    /*
     * @brief Removes the cards from the deck in the hand
     *
     * @return The number of cards removed from the deck
     */
    int remove_cards(const Hand& hand);

    /*
     * @brief Removes the card from the deck
     *
     * @return True if the card was removed, false otherwise
     */
    bool remove_card(const Card& card);

  private:
    int next_card_idx;
};

}  // namespace cribbage

#endif  // DECK_H
