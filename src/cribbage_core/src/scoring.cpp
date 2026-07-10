#include "scoring.h"

#include <cassert>
#include <algorithm>
#include <iostream>
#include <format>
#include <map>
#include <array>
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

using RankCounts = std::map<int, int>;

RankCounts hand_set(const Card& c1, const Card& c2,
                    const Card& c3, const Card& c4, Card cut) {
    RankCounts counts;
    ++counts[c1.get_rank_int()];
    ++counts[c2.get_rank_int()];
    ++counts[c3.get_rank_int()];
    ++counts[c4.get_rank_int()];
    ++counts[cut.get_rank_int()];
    return counts;
}

RankCounts hand_set(const Hand& hand, Card cut) {
    RankCounts counts;
    for (const Card& card : hand) {
        ++counts[card.get_rank_int()];
    }    
    ++counts[cut.get_rank_int()];
    return counts;
}

/*
* Iterates over all nonempty subsets of 5 cards ands calculates if
* they sum to 15
*/
int score_15(RankCounts counts) {
    std::array<int, 5> values{};

    std::size_t index = 0;
    for (const auto&[rank, count] : counts) {
        for (int i = 0; i < count; i++) {
            values[index++] = std::clamp(rank, 0, 10);
        }
    }

    int score = 0;
    int subsets = 31; // Five cards have 2^5 - 1 = 31 nonempty subsets.
    for (int subset = 1; subset <= subsets; ++subset) {
        int sum = 0;

        for (std::size_t card_index = 0;
             card_index < values.size();
             ++card_index) {

            bool is_in_subset = subset & (1u << card_index);
            if (is_in_subset) {
                sum += values[card_index];
            }
        }

        if (sum == 15) {
            score += 2;
        }
    }

    return score;
}

int calculate_runs(const RankCounts& counts) {
    int score = 0;

    char mult = 0;
    char length = 1;
    int last_rank = counts.begin()->first; // first is rank, second is count
    for (const auto&[rank, count] : counts) {
        if (rank - last_rank == 1)
            length++;
        else {
            if (length >= 3) {
                break; // No more runs possible
            }
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
    
    return score;
}

int calculate_pairs(const RankCounts& counts) {
    int score = 0;
    for (const auto&[_,count] : counts) {
        score += score_pair_count(count);
    }
    return score;
}

int calculate_runs_15s_pairs(const Card& c1, const Card& c2,
                            const Card& c3, const Card& c4, Card cut) {
    int score = 0;

    RankCounts counts = hand_set(c1, c2, c3, c4, cut);

    score += calculate_runs(counts);
    score += calculate_pairs(counts);
    score += score_15(counts);

    return score;
}

int calculate_runs_15s_pairs(const Hand& hand, Card cut) {
    int score = 0;

    RankCounts counts = hand_set(hand, cut);

    score += calculate_runs(counts);
    score += calculate_pairs(counts);
    score += score_15(counts);

    return score;
}

int get_or_set_cached_score(const Card& c1, const Card& c2, 
                            const Card& c3, const Card& c4, Card cut) {
    int hand_ranks[5];
    hand_ranks[0] = c1.get_rank_int();
    hand_ranks[1] = c2.get_rank_int();
    hand_ranks[2] = c3.get_rank_int();
    hand_ranks[3] = c4.get_rank_int();
    hand_ranks[4] = cut.get_rank_int();

    int key = get_hand_ranks_key(hand_ranks);
    if (score_table_cache.find(key) == score_table_cache.end()) {
        score_table_cache[key] = calculate_runs_15s_pairs(c1, c2, c3, c4, cut);
    }
    return score_table_cache[key];
}

int get_or_set_cached_score(const Hand& hand, Card cut) {
    if (hand.size() != 4) {
        std::cerr << "Hand must have 4 cards.\nHand: " << hand << std::endl;
        return 0;
    }
    int hand_ranks[5];
    int i = 0;
    for (const Card& card : hand) {
        hand_ranks[i++] = card.get_rank_int();
    }
    hand_ranks[i] = cut.get_rank_int();

    int key = get_hand_ranks_key(hand_ranks);
    if (score_table_cache.find(key) == score_table_cache.end()) {
        score_table_cache[key] = calculate_runs_15s_pairs(hand, cut);
    }
    return score_table_cache[key];
}

int score_runs_15s_pairs_cached(const Card& c1, const Card& c2,
                            const Card& c3, const Card& c4, const Card& cut) {
    int score = get_or_set_cached_score(c1, c2, c3, c4, cut);
    return score;
}

int score_runs_15s_pairs_cached(const Hand& hand, Card cut) {
    int score = get_or_set_cached_score(hand, cut);
    return score;
}

int score_runs_15s_pairs(const Hand& hand, Card cut) {
    return score_runs_15s_pairs_cached(hand, cut);
     // return calculate_runs_15s_pairs(hand, cut);
}

/*
 * Scores a flush for a given hand, cut, and is_crib
 *
 * is_crib is relavent since if there is a four card flush in the hand,
 * but the cut card doesn't match, it is not a 4 point flush for the crib.
 *
 */
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
    if (is_crib) { // Need a full 5 card flush for the crib
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
    return score_runs_15s_pairs_cached(hand, cut);
}

int score_hand(const Hand& hand, Card cut, bool is_crib) {
    char score = 0;
    score += score_runs_15s_pairs(hand, cut);
    score += score_flush(hand, cut, is_crib);
    score += score_knob(hand, cut);
    return score;
}

// For fast scoring in nested for loops
int score_crib(
    const Card& discard1,
    const Card& discard2,
    const Card& opponent1,
    const Card& opponent2,
    const Card& cut
) {
    const int rank_score = score_runs_15s_pairs_cached(
        discard1,
        discard2,
        opponent1,
        opponent2,
        cut
    );

    int score = rank_score;

    const Suit cut_suit = cut.get_suit();

    const bool crib_flush =
        discard1.get_suit() == cut_suit &&
        discard2.get_suit() == cut_suit &&
        opponent1.get_suit() == cut_suit &&
        opponent2.get_suit() == cut_suit;

    if (crib_flush) {
        score += 5;
    }

    const bool has_nobs =
        (discard1.get_rank() == Rank::JACK &&
         discard1.get_suit() == cut_suit) ||
        (discard2.get_rank() == Rank::JACK &&
         discard2.get_suit() == cut_suit) ||
        (opponent1.get_rank() == Rank::JACK &&
         opponent1.get_suit() == cut_suit) ||
        (opponent2.get_rank() == Rank::JACK &&
         opponent2.get_suit() == cut_suit);

    if (has_nobs) {
        ++score;
    }

    return score;
}
}  // namespace cribbage
