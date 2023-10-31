#include "deck.h"

#include <algorithm>
#include <random>

Deck::Deck()
{
    cards.reserve(52);
    make_deck();
}

void Deck::make_deck()
{
    cards.clear();
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 1; rank <= 13; ++rank) {
            cards.push_back(Card(static_cast<Suit>(suit), static_cast<Rank>(rank)));
        }
    }
}

void Deck::shuffle()
{
    // TODO: Make random number generator a class
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

Card Deck::deal_card()
{
    Card card = cards.back();
    cards.pop_back();
    return card;
}
