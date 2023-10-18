#include "card.h"

Card::Card(Suit suit, Rank rank)
    : suit(suit), rank(rank)
{
}

Suit Card::get_suit() const
{
    return suit;
}

Rank Card::get_rank() const
{
    return rank;
}

int Card::get_value() const
{
    if (static_cast<int>(rank) >= 10) {
        return 10;
    }
    return static_cast<int>(rank);
}

bool Card::operator==(const Card& rhs) const
{
    return suit == rhs.suit && rank == rhs.rank;
}

bool Card::compare_rank(const Card& lhs, const Card& rhs)
{
    return lhs.rank < rhs.rank;
}

bool Card::compare_suit(const Card& lhs, const Card& rhs)
{
    return lhs.suit < rhs.suit;
}

std::string Card::to_string() const
{
    std::string suitStr;
    switch (suit) {
        case Suit::CLUBS:
            suitStr = "C";
            break;
        case Suit::DIAMONDS:
            suitStr = "D";
            break;
        case Suit::HEARTS:
            suitStr = "H";
            break;
        case Suit::SPADES:
            suitStr = "S";
            break;
    }

    std::string rankStr;
    switch (rank) {
        case Rank::ACE:
            rankStr = "A";
            break;
        case Rank::TWO:
            rankStr = "2";
            break;
        case Rank::THREE:
            rankStr = "3";
            break;
        case Rank::FOUR:
            rankStr = "4";
            break;
        case Rank::FIVE:
            rankStr = "5";
            break;
        case Rank::SIX:
            rankStr = "6";
            break;
        case Rank::SEVEN:
            rankStr = "7";
            break;
        case Rank::EIGHT:
            rankStr = "8";
            break;
        case Rank::NINE:
            rankStr = "9";
            break;
        case Rank::TEN:
            rankStr = "10";
            break;
        case Rank::JACK:
            rankStr = "J";
            break;
        case Rank::QUEEN:
            rankStr = "Q";
            break;
        case Rank::KING:
            rankStr = "K";
            break;
    }

    return rankStr + suitStr;
}

std::ostream& operator<<(std::ostream& os, const Card& card)
{
    os << card.to_string();
    return os;
}
