#include "card.h"

#include "util/parse.h"

namespace cribbage {

Card::Card() : suit(Suit::NONE), rank(Rank::NONE) {}

Card::Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

Card::Card(std::string card_str) {
    *this = parse_card(card_str);
}

Suit Card::get_suit() const {
    return suit;
}

int Card::get_suit_int() const {
    return static_cast<int>(suit);
}

Rank Card::get_rank() const {
    return rank;
}

int Card::get_rank_int() const {
    return static_cast<int>(rank);
}

int Card::get_value() const {
    if (get_rank_int() >= 10) {
        return 10;
    }
    return get_rank_int();
}

bool Card::operator==(const Card& rhs) const {
    return suit == rhs.suit && rank == rhs.rank;
}

bool Card::compare_rank(const Card& lhs, const Card& rhs) {
    return lhs.rank < rhs.rank;
}

bool Card::compare_suit(const Card& lhs, const Card& rhs) {
    return lhs.suit < rhs.suit;
}

std::string Card::to_string() const {
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
        case Suit::NONE:
            suitStr = "N";
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
            rankStr = "T";
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
        case Rank::NONE:
            rankStr = "N";
            break;
    }

    return rankStr + suitStr;
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
    os << card.to_string();
    return os;
}

}  // namespace cribbage
