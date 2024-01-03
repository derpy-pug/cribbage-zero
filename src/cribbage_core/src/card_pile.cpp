#include "card_pile.h"

#include <algorithm>

#include "scoring.h"

namespace cribbage {

CardPile::CardPile() : current_sum(0), current_round_offset(0) {}

void CardPile::add_card(Card card) {
    if (!can_play_card(card)) {
        next_round();
    }
    cards.push_back(card);
    current_sum += card.get_value();
    scores.push_back(score_pile());
}

bool CardPile::can_play_card(Card card) const {
    return current_sum + card.get_value() <= 31;
}

int CardPile::score_pile_run() const {
    int score = 0;
    int min = 99;
    int max = 0;
    int length = 0;
    bool double_card[14] = {false};
    for (auto it = cards.rbegin(); it != cards.rend() - current_round_offset;
         it++) {
        // If we have a double card, then we can't have a run
        if (double_card[it->get_rank_int()])
            break;
        double_card[it->get_rank_int()] = true;

        length++;
        min = std::min(min, it->get_rank_int());
        max = std::max(max, it->get_rank_int());
        if (length >= 3 && max - min == length - 1) {
            score = length;
        }
    }
    return score;
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
    for (int i = cards.size() - 1; i >= current_round_offset; i--) {
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
    if (!can_play_card(another_card)) {
        std::cerr << "Cannot add card " << another_card << " to pile " << *this
                  << std::endl;
        return 0;
    }
    add_card(another_card);
    scores.pop_back();
    int score = score_pile();
    cards.pop_back();
    return score;
}

void CardPile::next_round() {
    previous_round_offset = current_round_offset;
    current_round_offset = cards.size();
    current_sum = 0;
}

std::string CardPile::to_string() const {
    std::string result;
    int i = 0;
    for (auto card : cards) {
        bool is_next_round =
          current_round_offset != 0 &&  // Don't add | at the beginning
          (i == current_round_offset || (i == previous_round_offset && i != 0));
        if (is_next_round) {
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

}  // namespace cribbage
