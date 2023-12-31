#include "hand.h"

#include <algorithm>

namespace cribbage {

Hand::Hand() {
    cards.reserve(6);
}

Hand::Hand(std::vector<Card> cards) {
    this->cards = cards;
}

void Hand::add_card(Card card) {
    cards.push_back(card);
}

void Hand::remove_card(Card card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
}

bool Hand::contains(Card card) const {
    return std::find(cards.begin(), cards.end(), card) != cards.end();
}

void Hand::sort() {
    auto comparator = Card::compare_rank;
    std::sort(cards.begin(), cards.end(), comparator);
}

Hand Hand::sort() const {
    Hand sorted;
    auto cards_copy = cards;
    auto comparator = Card::compare_rank;
    std::sort(cards_copy.begin(), cards_copy.end(), comparator);
    sorted.cards = cards_copy;
    return sorted;
}

bool Hand::operator==(const Hand& other) const {
    Hand sorted = sort();
    Hand other_sorted = other.sort();
    return sorted.cards == other_sorted.cards;
}

bool Hand::operator!=(const Hand& other) const {
    return !(*this == other);
}

std::string Hand::to_string() const {
    std::string str;
    str.reserve(3 * cards.size());
    for (const Card& card : cards) {
        str += card.to_string() + " ";
    }
    str.pop_back();
    return str;
}

std::ostream& operator<<(std::ostream& os, const Hand& hand) {
    os << hand.to_string();
    return os;
}

}  // namespace cribbage
