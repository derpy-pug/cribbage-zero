#ifndef CARD_H
#define CARD_H

#include <iostream>

enum class Suit { CLUBS = 0, DIAMONDS, HEARTS, SPADES };

enum class Rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };

class Card
{
public:
    /*
     * @brief Constructs a card with the given suit and rank
     *
     * @param suit The suit of the card
     * @param rank The rank of the card
     */
    Card(Suit suit, Rank rank);

    /*
     * @brief Returns the suit of the card
     *
     * @return The suit of the card
     */
    Suit get_suit() const;

    /*
     * @brief Returns the rank of the card
     *
     * @return The rank of the card
     */
    Rank get_rank() const;

    /*
     * @brief Returns the cribbage value of the card between 1 and 10
     *
     * The cribbage value of a card is its rank, with the exception that face
     * cards are worth 10.
     *
     * @return The cribbage value of the card between 1 and 10
     */
    int get_value() const;

    /*
     * @brief Returns the string representation of the card
     *
     * The string representation of a card is its rank followed by its suit.
     * For example, the string representation of the ace of spades is "AS".
     *
     * @return The string representation of the card
     */
    std::string print() const;

    // Print the card to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const Card& card);

private:
    Suit suit;
    Rank rank;
};

#endif // CARD_H
