#include "deck.h"

#include <algorithm>
#include "cribbage_random.h"

namespace cribbage {

Deck::Deck() : next_card_idx(51) {
    cards.reserve(52);
    make_deck();
}

void Deck::make_deck() {
    cards.clear();
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 1; rank <= 13; ++rank) {
            cards.emplace_back(
                Card(static_cast<Suit>(suit), static_cast<Rank>(rank)));
        }
    }
    next_card_idx = 51;
}

void Deck::shuffle() {
    auto g = CribbageRandom::get_instance()->get_generator();
    std::shuffle(cards.begin(), cards.end(), g);
    next_card_idx = cards.size() - 1;
}

void Deck::shuffleTopCardsIntoDeck() {
    for (int i = cards.size() - 1; i > next_card_idx; --i) {
        int ri = CribbageRandom::get_instance()->get_int(0, i);
        std::swap(cards[i], cards[ri]);
    }
    next_card_idx = cards.size() - 1;
}

Card Deck::deal_card() {
    if (next_card_idx < 0) {
        std::cerr << "No more cards in deck. Undefined Behavior." << std::endl;
    }
    Card card = cards[next_card_idx];
    --next_card_idx;
    return card;
}

Card Deck::deal_card_and_remove() {
    if (next_card_idx >= (int)cards.size()) {
        std::cerr << "No more cards in deck. Undefined Behavior." << std::endl;
    }
    Card card = cards[next_card_idx];
    cards.erase(cards.begin() + next_card_idx);
    --next_card_idx;
    return card;
}

Hand Deck::deal_hand(int numCards) {
    Hand hand;
    for (int i = 0; i < numCards; ++i) {
        hand.add_card(deal_card());
    }
    return hand;
}

Hand Deck::deal_hand_and_remove(int numCards) {
    if (next_card_idx + 1 < numCards) {
        std::cerr << "Not enough cards in deck. Undefined Behavior."
                  << std::endl;
    }
    Hand hand;
    for (int i = 0; i < numCards; ++i) {
        hand.add_card(cards[next_card_idx - i]);
    }
    cards.erase(cards.begin() + next_card_idx - numCards + 1,
                cards.begin() + next_card_idx + 1);
    next_card_idx -= numCards;
    return hand;
}

int Deck::remove_cards(const Hand& hand) {
    // This function return should be used
    // How?
    auto num_erased = std::erase_if(
        cards, [&hand](const Card& card) { return hand.contains(card); });
    next_card_idx -= num_erased;
    return num_erased;
}

bool Deck::remove_card(const Card& card) {
    cards.erase(std::remove(cards.begin(), cards.end(), card), cards.end());
    --next_card_idx;
    return true;
}

}  // namespace cribbage
