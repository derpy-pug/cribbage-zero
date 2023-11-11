#include "card_pile.h"

#include <algorithm>

#include "scoring.h"

CardPile::CardPile() : current_sum(0), round_offset(0) {}

void CardPile::add_card(Card card) {
    if (!can_add_card(card)) {
        std::cerr << "Cannot add card " << card << " to pile " << *this
                  << std::endl;
        return;
    }
    cards.push_back(card);
    current_sum += card.get_value();
}

bool CardPile::can_add_card(Card card) const {
    return current_sum + card.get_value() <= 31;
}

int CardPile::score_pile_run() const {
    char min;
    char max;
    int length = cards.size() - round_offset;
    for (int i = 0; i < length; i++) {
        min = static_cast<int>(cards[i].get_rank());
        max = static_cast<int>(cards[i].get_rank());
        for (int j = i + 1; j < length; j++) {
            char rank = static_cast<int>(cards[j].get_rank());
            if (rank < min)
                min = rank;
            if (rank > max)
                max = rank;
        }
        if (max - min == length - i - 1) {
            return max - min;
        }
    }
    return 0;
}

int CardPile::score_pile() const {
    int score = 0;

    // Score 31
    if (current_sum == 31) {
        score += 1;
    }
    // Score 15
    if (current_sum == 15) {
        score += 2;
    }
    // Score pairs
    int pairs = 1;
    for (int i = cards.size() - 1; i >= round_offset; i--) {
        if (cards[i].get_rank() == cards[i - 1].get_rank()) {
            pairs++;
        } else {
            score += score_pair_count(pairs);
            break;
        }
    }
    // Score runs
    score += score_pile_run();
    return score;
}

int CardPile::score_pile(Card another_card) {
    if (!can_add_card(another_card)) {
        std::cerr << "Cannot add card " << another_card << " to pile " << *this
                  << std::endl;
        return 0;
    }
    add_card(another_card);
    int score = score_pile();
    cards.pop_back();
    return score;
}

void CardPile::next_round() {
    round_offset += cards.size();
    current_sum = 0;
}

std::string CardPile::to_string() const {
    std::string result;
    int i = 0;
    for (auto card : cards) {
        if (i == round_offset) {
            result += "| ";
        }
        result += card.to_string() + " ";
        i++;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const CardPile& pile) {
    os << pile.to_string();
    return os;
}
