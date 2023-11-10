#include "card.h"

Card::Card(Suit suit, Rank rank)
    : suit(suit), rank(rank)
{
}

Card::Card(std::string card_str)
{
    if (card_str.size() != 2) {
        throw std::invalid_argument("Card string must be 2 characters");
    }

    // Upper case the string
    char rank_char = std::toupper(card_str[0]);
    char suit_char = std::toupper(card_str[1]); 

    switch (rank_char) {
        case 'A':
        case '1':
            rank = Rank::ACE;
            break;
        case '2':
            rank = Rank::TWO;
            break;
        case '3':
            rank = Rank::THREE;
            break;
        case '4':
            rank = Rank::FOUR;
            break;
        case '5':
            rank = Rank::FIVE;
            break;
        case '6':
            rank = Rank::SIX;
            break;
        case '7':
            rank = Rank::SEVEN;
            break;
        case '8':
            rank = Rank::EIGHT;
            break;
        case '9':
            rank = Rank::NINE;
            break;
        case 'T':
            rank = Rank::TEN;
            break;
        case 'J':
            rank = Rank::JACK;
            break;
        case 'Q':
            rank = Rank::QUEEN;
            break;
        case 'K':
            rank = Rank::KING;
            break;
        default:
            throw std::invalid_argument("Invalid rank character");
    }

    switch (suit_char) {
        case 'C':
            suit = Suit::CLUBS;
            break;
        case 'D':
            suit = Suit::DIAMONDS;
            break;
        case 'H':
            suit = Suit::HEARTS;
            break;
        case 'S':
            suit = Suit::SPADES;
            break;
        default:
            throw std::invalid_argument("Invalid suit character");
    }
}

Suit Card::get_suit() const
{
    return suit;
}

Rank Card::get_rank() const
{
    return rank;
}

int Card::get_rank_int() const
{
    return static_cast<int>(rank);
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
