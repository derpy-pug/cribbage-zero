#include "scoring.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace cribbage {

// Hash the score of runs, 15s, and pairs
static std::unordered_map<int, int> score_table_cache;

// Used for getting the key for the score table
static char primes[14] = {0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

int get_hand_ranks_key(const int hand[5]) {
    int key = 1;
    for (int i = 0; i < 5; i++) {
        key *= primes[hand[i]];
    }
    return key;
}

/*
 * @brief scores a pair based on the number of cards in the pair
 *
 * Pairs are scored as follows:
 * - 2 cards: 2 points
 * - 3 cards: 6 points
 * - 4 cards: 12 points
 * - Everything else: 0 points
 *
 * @param count The number of cards in the pair
 *
 * @return The score of the pair
 */
int score_pair_count(int count) {
    if (count == 2)
        return 2;
    if (count == 3)
        return 6;
    if (count == 4)
        return 12;
    return 0;
}

/*
 * @brief calculates the set of cards in the deck and their counts
 *
 * Used to calculate runs, 15s, and pairs
 *
 * Example: 5H 5D 5C 5S 6H  
 * set = [(5, 4), (6, 1)]
 *
 * @param hand The hand to calculate the set of
 * @param cut The cut card
 *
 * @return A vector of pairs of the form (rank, count). Sorted by rank.
 */
std::vector<std::pair<int, int>> hand_set(const Hand& hand, Card cut) {
    std::vector<std::pair<int, int>> set;
    std::vector<int> rank_counts(14, 0);
    for (const Card& card : hand) {
        int rank = card.get_rank_int();
        ++rank_counts[rank];
    }
    int rank = cut.get_rank_int();
    ++rank_counts[rank];

    for (int i = 1; i < 14; ++i) {
        if (rank_counts[i] > 0) {
            auto pair = std::make_pair(i, rank_counts[i]);
            set.emplace_back(pair);
        }
    }
    return set;
}

int score_15(const Hand& hand, Card cut) {
    int score = 0;
    char interactions[32] = {0};
    int k = 0;

    for (const Card& card : hand) {
        int value = static_cast<int>(card.get_value());
        int k_temp = k;
        for (int j = 0; j < k_temp; j++) {
            int sum = interactions[j] + value;
            if (sum == 15)
                score += 2;
            if (sum < 15)
                interactions[k++] = sum;
        }
        interactions[k++] = value;
    }

    // Handle cut card
    for (int j = 0; j < k; j++) {
        int sum = interactions[j] + static_cast<int>(cut.get_value());
        if (sum == 15)
            score += 2;
    }
    return score;
}

int calculate_runs_15s_pairs(const Hand& hand, Card cut) {
    int score = 0;

    std::vector<std::pair<int, int>> set = hand_set(hand, cut);

    char mult = 0;
    char length = 1;
    int last_rank = set[0].first;
    for (const auto&[rank, count] : set) {
        if (rank - last_rank == 1)
            length++;
        else {
            if (length >= 3)
                break;
            mult = 0;
            length = 1;
        }
        if (count > 1)
            mult += count;
        last_rank = rank;
    }
    if (mult == 0)
        mult = 1;
    if (length >= 3)
        score += mult * length;

    for (const auto&[_,count] : set) {
        score += score_pair_count(count);
    }

    score += score_15(hand, cut);
    return score;
}

int get_hashed_score(const Hand& hand, Card cut) {
    if (hand.size() != 4) {
        std::cerr << "Hand must have 4 cards.\nHand: " << hand << std::endl;
        return 0;
    }
    int hand_ranks[5];
    int i = 0;
    for (const Card& card : hand) {
        hand_ranks[i++] = static_cast<int>(card.get_rank());
    }
    hand_ranks[i] = static_cast<int>(cut.get_rank());

    int key = get_hand_ranks_key(hand_ranks);
    if (score_table_cache.find(key) == score_table_cache.end()) {
        score_table_cache[key] = calculate_runs_15s_pairs(hand, cut);
    }
    return score_table_cache[key];
}

int score_runs_15s_pairs(const Hand& hand, Card cut) {
    int score = get_hashed_score(hand, cut);
    return score;
}

int score_flush(const Hand& hand, Card cut, bool is_crib) {
    Suit suit = hand[0].get_suit();
    bool not_flush = std::any_of(hand.begin(), hand.end(), [suit](const Card& card) {
        return card.get_suit() != suit;
    });
    if (not_flush) {
        return 0;
    }
    if (cut.get_suit() == suit) {
        return 5;
    }
    if (is_crib) {
        return 0;
    }
    return 4;
}

int score_knob(const Hand& hand, Card cut) {
    return std::any_of(hand.begin(), hand.end(), [cut](const Card& card) {
        return card.get_rank() == Rank::JACK && card.get_suit() == cut.get_suit();
    });
}

int score_hand_suitless(const Hand& hand, Card cut) {
    return score_runs_15s_pairs(hand, cut);
}

int score_hand(const Hand& hand, Card cut, bool is_crib) {
    char score = 0;
    score += score_runs_15s_pairs(hand, cut);
    score += score_flush(hand, cut, is_crib);
    score += score_knob(hand, cut);
    return score;
}

}  // namespace cribbage
