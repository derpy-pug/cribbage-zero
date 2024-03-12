#include "statistics/stats/CribDiscardProbabilities.h"

namespace cribbage {

CribDiscardProbabilities::CribDiscardProbabilities()
    : table("Crib Discard Probabilities"), num_sim_discards(0) {}

CribDiscardProbabilities::CribDiscardProbabilities(int num_sim_discards)
    : table("Crib Discard Probabilities"), num_sim_discards(num_sim_discards) {}

const float& CribDiscardProbabilities::get_prob(Card card1, Card card2,
                                                bool is_dealer) const {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

const float& CribDiscardProbabilities::get_prob(Rank rank1, Rank rank2,
                                                bool is_dealer) const {
    int index1 = static_cast<int>(rank1) - 1;
    int index2 = static_cast<int>(rank2) - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

const float& CribDiscardProbabilities::get_table_value(int index1, int index2,
                                                       bool is_dealer) const {
    if (is_dealer) {
        return table.get_dealer_crib(index1, index2);
    } else {
        return table.get_opp_crib(index1, index2);
    }
}

float& CribDiscardProbabilities::get_table_value(int index1, int index2,
                                                 bool is_dealer) {
    if (is_dealer) {
        return table.get_dealer_crib(index1, index2);
    } else {
        return table.get_opp_crib(index1, index2);
    }
}

float& CribDiscardProbabilities::get_table_value(Rank rank1, Rank rank2,
                                                 bool is_dealer) {
    int index1 = static_cast<int>(rank1) - 1;
    int index2 = static_cast<int>(rank2) - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

void CribDiscardProbabilities::to_freq_table() {
    if (is_freq_table) {
        return;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            table.get_dealer_crib(i, j) *= num_sim_discards;
            table.get_opp_crib(i, j) *= num_sim_discards;
        }
    }
    is_freq_table = true;
}

void CribDiscardProbabilities::to_prob_table() {
    if (!is_freq_table) {
        return;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            table.get_dealer_crib(i, j) /= num_sim_discards;
            table.get_opp_crib(i, j) /= num_sim_discards;
        }
    }
    is_freq_table = false;
}

} // namespace cribbbage
