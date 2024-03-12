#pragma once

#include <optional>
#include <string>

#include "card.h"
#include "ScoreDistributionTable.h"
#include "CribDiscardProbabilities.h"


namespace cribbage {

class StatisticsTable {
  public:
    StatisticsTable();
    //~StatisticTable() = default;

    friend class GenerateCribStatistics;

    int load_tables(std::optional<std::string> table = std::nullopt);
    void save_tables(std::string table);

    const CribDiscardProbabilities& get_crib_discard_probs() {
        return prob_table;
    }
    ScoreDistributionTable get_score_dist(Card card1, Card card2,
                                          bool is_dealer);
    float get_mean(Card card1, Card card2, bool is_dealer);
    int get_median(Card card1, Card card2, bool is_dealer);
    float get_variance(Card card1, Card card2, bool is_dealer);
    float get_std_dev(Card card1, Card card2, bool is_dealer);
    int get_max(Card card1, Card card2, bool is_dealer);
    int get_min(Card card1, Card card2, bool is_dealer);

  private:
    void load_default_tables();

  protected:
    CribDiscardProbabilities prob_table;

    // Used to calculate the other tables
    Table<ScoreDistributionTable> score_dist_table;

    Table<float> mean_table;
    Table<int> median_table;
    Table<float> variance_table;
    Table<float> std_dev_table;
    Table<int> max_table;
    Table<int> min_table;
};

} // namespace cribbage
