#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H

#include <optional>

#include "player.h"
#include "statistics.h"

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

    StatisticTable& get_stat_table() { return stat_table; } 

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
    StatisticTable stat_table;
};

class GenerateDiscardHeuristics {
  public:
    GenerateDiscardHeuristics(const AllDiscardStatistics& all_discard_stats); 

    DiscardHeuristics generate_discard_heuristics(ScoreType score_type = ScoreType::COMBINED,
                                                  Statistic stat = Statistic::MEAN) const;

  private:
    const AllDiscardStatistics& all_discard_stats;
};


class GenerateDiscardStatistics {
  public:
    /*
     * @param player The player to generate discard statistics for. They should 
     *             have a hand of 6 cards.
     * @param is_dealer True if the player is the dealer, false otherwise.
     * @param gen_crib_stats The crib statistics tables to use.
     */
    GenerateDiscardStatistics(
      Player* player, bool is_dealer,
      const CribDiscardProbabilities& crib_discard_probs);

    /*
     * @brief Generate discard statistics for all possible discard combinations.
     *
     * @param cut The cut card. If not provided, all possible cut cards will be
     *           used.
     */
    void generate_all_discard_stats(std::optional<Card> cut = std::nullopt,
                                    bool use_simulated = false,
                                    Player* opponent = nullptr);

    /*
     * @brief generates the heuristics for the discard statistics.
     *
     * @important This should be called after generate_all_discard_stats() has
     *       been called.
     */
    void generate_heuristics(ScoreType score_type = ScoreType::COMBINED,
                             Statistic stat = Statistic::MEAN);

    AllDiscardStatistics get_all_discard_stats() const {
        return all_discard_stats;
    }

  private:
    /*
     * @brief Generate discard statistics for all possible cut cards and
     *        all possible opponent discards.
     *
     * @param cut The cut card. If not provided, all possible cut cards will be
     *          used.
     *
     * Tables are generated using the crib statistics tables. Specifically, the
     * frequency tables. 
     */
    DiscardStatistics generate_discard_stats(
      Card discard1, Card discard2, std::optional<Card> cut = std::nullopt);

    /*
     * @brief Generate discard statistics for all possible cut cards and
     *        all possible opponent discards.
     *
     * @param opponent The opponent to generate discard statistics for. They
     *                only need a discard function.
     *
     * @param cut The cut card. If not provided, all possible cut cards will be
     *          used.
     *
     * Tables are generated by simulating all possible cut cards and all
     * possible opponent discards. This is much slower than using the crib 
     * statistics tables. But, it is more accurate.
     */
    DiscardStatistics generate_discard_stats_simulated(
      Card discard1, Card discard2, std::optional<Card> cut = std::nullopt,
      Player* opponent = nullptr);

  private:
    AllDiscardStatistics all_discard_stats;
    Player* player;
    bool is_dealer;
    const CribDiscardProbabilities& crib_discard_probs;
};

}  // namespace cribbage

#endif  // GENERATE_STATISTICS_H
