#include "deck.h"

#include <algorithm>
#include "util/cribbage_random.h"

Deck::Deck()
    : next_card_idx(0)
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
    auto g = CribbageRandom::get_instance()->get_generator();
    std::shuffle(cards.begin(), cards.end(), g);
    next_card_idx = 0;
}

void Deck::shuffleTopCardsIntoDeck(int numCards) {
    if (numCards > (int)cards.size()) {
        shuffle();
        return;
    }
    for (int i = 0; i < numCards; ++i) {
        int ri = CribbageRandom::get_instance()->get_random_int(i, cards.size());
        std::swap(cards[i], cards[ri]);
    }
}

Card Deck::deal_card()
{
    if (next_card_idx >= (int)cards.size()) {
        std::cerr << "No more cards in deck. Undefined Behavior." << std::endl;
    }
    Card card = cards[next_card_idx++];
    return card;
}

Hand Deck::deal_hand(int numCards)
{
    Hand hand;
    for (int i = 0; i < numCards; ++i) {
        hand.add_card(deal_card());
    }
    return hand;
}
