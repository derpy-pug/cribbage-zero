#pragma once

#include "card.h"
#include "ScoreDistributionTable.h"

namespace cribbage {

/*
 * @brief Table for storing the number of times a score occurs.
 */
class CribDiscardProbabilities {
  public:
    CribDiscardProbabilities();
    CribDiscardProbabilities(int num_sim_discards);

    friend class StatisticsTable;
    friend class GenerateCribStatistics;

    const float& get_prob(Card card1, Card card2, bool is_dealer) const;
    const float& get_prob(Rank rank1, Rank rank2, bool is_dealer) const;

  private:
    const float& get_table_value(int index1, int index2, bool is_dealer) const;
    float& get_table_value(int index1, int index2, bool is_dealer);
    float& get_table_value(Rank rank1, Rank rank2, bool is_dealer);

    void to_freq_table();
    void to_prob_table();

  private:
    Table<float> table;
    int num_sim_discards;
    bool is_freq_table = false;
};

} // namespace cribbage
