#pragma once

#include "player.h"

#include "statistics/stats/StatisticsTable.h"

namespace cribbage {

class GenerateCribStatistics {
  public:
    GenerateCribStatistics(Player* dealer, Player* pone);

    /* 
     * @brief Generates the probabilities for every possible discard pair based
     *       on the the players.
     *
     * @important This should be called before calculating any statistics.
     */
    void generate_probabilities();

    /*
     * @brief Calculates all statistics for every possible discard pair.
     *
     * @note these are optional statistics and they are not used by any core
     *      functions.
     */
    void calculate_stats();

    StatisticsTable& get_stat_table() { return stat_table; } 

    CribDiscardProbabilities& get_crib_discard_probs() {
        return stat_table.prob_table;
    }

    /*
     * @brief Gets the mean of a discard but also takes into account the
     *       probability of the crib getting a flush.
     *
     * @note this is not used by any core functions.
     */
    [[deprecated]] float get_mean_counting_flush(Card card1, Card card2,
                                                 bool is_dealer,
                                                 int num_flush_cards);

  private:
    void generate_discard_probabilities();
    void calculate_discard_score_distribution();

    void calculate_other_stats();

    void calculate_max_min_tables();
    void calculate_mean_tables();
    void calculate_median_tables();

    void calculate_variance_tables();

    void calculate_std_dev_tables();

  private:
    Player* dealer;
    Player* pone;
    StatisticsTable stat_table;
};

}  // namespace cribbage
