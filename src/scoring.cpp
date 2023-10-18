#include "scoring.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>

// Used for getting the key for the score table
static char primes[14] = { 0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };

int get_hand_ranks_key(int hand[5]) {
    int key = 1; 
    for (int i = 0; i < 5; i++) {
        key *= primes[hand[i]];
    }
    return key;
}

// Hash the score of runs, 15s, and pairs
static std::unordered_map<int, int> score_table;

int score_pair_count(int count)
{
    if (count == 2) return 2;
    if (count == 3) return 6;
    if (count == 4) return 12;
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
std::vector<std::pair<int, int>> hand_set(const Hand& hand, Card cut)
{
    std::vector<std::pair<int, int>> set;
    std::vector<int> rank_counts(14, 0);
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        Card card = *it;
        int rank = static_cast<int>(card.get_rank());
        ++rank_counts[rank];
    }
    int rank = static_cast<int>(cut.get_rank());
    ++rank_counts[rank];

    for (int i = 0; i < 14; ++i) {
        if (rank_counts[i] > 0) {
            set.push_back(std::make_pair(i, rank_counts[i]));
        }
    }
    return set;
} 

int score_15(const Hand& hand, Card cut)
{
    int score = 0;
	char interactions[32] = {0};
	int k = 0;
	for (auto it = hand.begin(); it != hand.end(); ++it) {
        int value = static_cast<int>(it->get_value()); 
		int k_temp = k;
		for (int j = 0; j < k_temp; j++) {
			int sum = interactions[j] + value;
			if (sum == 15) score += 2;
			if (sum < 15) interactions[k++] = sum;
		}
		interactions[k++] = value;
	}
    // Handle cut card
    for (int j = 0; j < k; j++) {
        int sum = interactions[j] + static_cast<int>(cut.get_rank());
        if (sum == 15) score += 2;
    }
    return score;
}

int calculate_runs_15s_pairs(const Hand& hand, Card cut)
{
    int score = 0;

    std::vector<std::pair<int, int>> set = hand_set(hand, cut);

	char mult = 0;
	char length = 1;
    int last_rank = set[0].first;
	for(auto pair : set) {
        int rank = pair.first;
        int count = pair.second;
		if (count > 1) 
            mult += count;
		if (rank - last_rank == 1)
			length++;
		else { 
			if (length >= 3) break;
			mult = 0;
			length = 1;
		}
        last_rank = rank;
	}
	if (mult == 0) mult = 1;
	if (length >= 3) score += mult * length;

	for(auto pair : set) {
        int count = pair.second;
		score += score_pair_count(count);
	}


	score += score_15(hand, cut);
    return score;
}

int get_hashed_score(const Hand& hand, Card cut)
{
    if (hand.size() != 4) {
        std::cerr << "Hand must have 4 cards" << std::endl;
        return 0;
    }
    int hand_ranks[5];
    int i = 0;
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        hand_ranks[i++] = static_cast<int>(it->get_rank());
    }
    hand_ranks[i] = static_cast<int>(cut.get_rank());

    int key = get_hand_ranks_key(hand_ranks);
    if (score_table.find(key) == score_table.end()) {
        score_table[key] = calculate_runs_15s_pairs(hand, cut);
    }
    return score_table[key];
}

int score_runs_15s_pairs(const Hand& hand, Card cut)
{
    int score = get_hashed_score(hand, cut);
    return score;
}

int score_flush(const Hand& hand, Card cut, bool is_crib)
{
    Suit suit = hand.begin()[0].get_suit();
    for (auto it = hand.begin() + 1; it != hand.end(); ++it) {
        Card card = *it;
        if (card.get_suit() != suit) {
            return 0;
        }
    }
    if (cut.get_suit() == suit) {
        return 5;
    }
    if (is_crib) {
        return 0;
    }
    return 4;
}

int score_knob(const Hand& hand, Card cut)
{
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        Card card = *it;
        if (card.get_rank() == Rank::JACK && card.get_suit() == cut.get_suit()) {
            return 1;
        }
    }
    return 0;
}

int score_hand(const Hand& hand, Card cut, bool is_crib)
{
    char score = 0;
    score += score_runs_15s_pairs(hand, cut);
	score += score_flush(hand, cut, is_crib);
	score += score_knob(hand, cut);
	return score;
}
