#ifndef CARD_H
#define CARD_H

#include <iostream>

namespace cribbage {

enum class Suit { CLUBS = 0, DIAMONDS, HEARTS, SPADES };

enum class Rank {
    ACE = 1,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING
};

class Card {
  public:
    /*
     * @brief Constructs a card with the given suit and rank
     *
     * @param suit The suit of the card
     * @param rank The rank of the card
     */
    Card(Suit suit, Rank rank);

    /*
     * @brief Constructs a card from a string representation
     *
     * The string representation of a card is its rank followed by its suit.
     * Examples:
     *  - "AS" is the ace of spades
     *  - "1S" is the ace of spades
     *  - "7H" is the seven of hearts
     *  - "TH" is the ten of hearts
     */
    Card(std::string card_str) noexcept(false);

    /*
     * @brief Returns the suit of the card
     *
     * @return The suit of the card
     */
    Suit get_suit() const;

    /*
     * @brief Returns the suit of the card as an integer
     *
     * Clubs is 0, diamonds is 1, hearts is 2, and spades is 3.
     *
     * @return The suit of the card as an integer
     */
    int get_suit_int() const;

    /*
     * @brief Returns the rank of the card
     *
     * @return The rank of the card
     */
    Rank get_rank() const;

    /* 
     * @brief Returns the rank of the card as an integer
     *
     * Aces are 1, twos are 2, ..., Ten is 10, Jack is 11, Queen is 12, and
     * King is 13.
     *
     * @return The rank of the card as an integer
     */
    int get_rank_int() const;

    /*
     * @brief Returns the cribbage value of the card between 1 and 10
     *
     * The cribbage value of a card is its rank, with the exception that face
     * cards are worth 10.
     *
     * @return The cribbage value of the card between 1 and 10
     */
    int get_value() const;

    bool operator==(const Card& rhs) const;

    static bool compare_rank(const Card& lhs, const Card& rhs);
    static bool compare_suit(const Card& lhs, const Card& rhs);

    /*
     * @brief Returns the string representation of the card
     *
     * The string representation of a card is its rank followed by its suit.
     * For example, the string representation of the ace of spades is "AS".
     *
     * @return The string representation of the card
     */
    std::string to_string() const;

    // Print the card to the given output stream
    friend std::ostream& operator<<(std::ostream& os, const Card& card);

  private:
    Suit suit;
    Rank rank;
};

}  // namespace cribbage

#endif  // CARD_H
