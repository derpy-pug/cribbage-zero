#ifndef STATISTICS_H
#define STATISTICS_H

#include <optional>
#include <string>
#include <vector>
#include "card.h"
#include "player.h"

namespace cribbage {

enum class ScoreType { HAND, CRIB, COMBINED };

enum class Statistic {
    MEAN,
    MEDIAN,
    VARIANCE,
    STD_DEV,
    MAX,
    MIN,
};

/*
 * @brief Table for storing the number of times a score occurs.
 */
class ScoreDistributionTable {
  public:
    ScoreDistributionTable();
    ScoreDistributionTable(int min, int max);

    ScoreDistributionTable(const ScoreDistributionTable& other) = default;
    ScoreDistributionTable(ScoreDistributionTable&& other) = default;
    ScoreDistributionTable& operator=(ScoreDistributionTable&& other) = default;

    float calc_mean() const;
    int calc_median() const;
    float calc_variance() const;
    float calc_std_dev() const;
    int calc_max() const;
    int calc_min() const;

    float prob_score(int score) const;
    float prob_score_cummulative(int score) const;
    float prob_score_between(int score1, int score2) const;

    float& operator[](int score);
    const float& operator[](int score) const;

    int get_possible_score_min() const { return possible_min; }
    int get_possible_score_max() const { return possible_max; }

    void clear();

    void normalize();

  private:
    float& get_table_value(int score);
    const float& get_table_value(int score) const;

  private:
    std::vector<float>
      dist_table;  // 0-29 for single hand, -60 through 60 for combined
    int possible_min;
    int possible_max;
    int min_score;
    int max_score;
};

template <typename T>
class Table {
  public:
    /*
     * @brief initializes all values to default.
     */
    Table();

    /*
     * @brief Constructor with stat name used for printing.
     */
    Table(std::string stats_name);

    T& get_dealer_crib(int index1, int index2) {
        return table_dealer_crib[index1][index2];
    }
    const T& get_dealer_crib(int index1, int index2) const {
        return table_dealer_crib[index1][index2];
    }
    T& get_opp_crib(int index1, int index2) {
        return table_opp_crib[index1][index2];
    }
    const T& get_opp_crib(int index1, int index2) const {
        return table_opp_crib[index1][index2];
    }

    void pretty_print();

    void save(std::string filename);
    int load(std::string filename);

  private:
    std::string stats_name;
    T table_dealer_crib[13][13];
    T table_opp_crib[13][13];
};

/*
 * @brief Table for storing the number of times a score occurs.
 */
class CribDiscardProbabilities {
  public:
    CribDiscardProbabilities();
    CribDiscardProbabilities(int num_sim_discards);

    friend class StatisticTable;
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

class DiscardStatistics {
  public:
    DiscardStatistics(Card discard1, Card discard2, bool is_dealer,
                      PlayerInfo player);

    friend class GenerateDiscardStatistics;

    inline Card get_discard1() const { return discard1; }
    inline Card get_discard2() const { return discard2; }

    const ScoreDistributionTable& get_score_dist(ScoreType score_type) const;

    float get_mean(ScoreType score_type) const;
    int get_median(ScoreType score_type) const;
    float get_variance(ScoreType score_type) const;
    float get_std_dev(ScoreType score_type) const;
    int get_max(ScoreType score_type) const;
    int get_min(ScoreType score_type) const;

    float get_prob(ScoreType score_type, int score) const;
    float get_prob_cummulative(ScoreType score_type, int score) const;
    float get_prob_between(ScoreType score_type, int score1, int score2) const;

    std::string get_discard_string() const;
    friend std::ostream& operator<<(std::ostream& os,
                                    const DiscardStatistics& discard_stats);

  private:
    ScoreDistributionTable& get_score_dist(ScoreType score_type);

  private:
    Card discard1;
    Card discard2;
    PlayerInfo player_info;
    bool is_dealer;

    ScoreDistributionTable score_dist_hand;
    ScoreDistributionTable score_dist_crib;
    ScoreDistributionTable score_dist_combined;
};

class AllDiscardStatistics {
  public:
    AllDiscardStatistics();

    friend class GenerateDiscardStatistics;

    void sort_discard_stats(ScoreType score_type = ScoreType::COMBINED,
                            Statistic stat = Statistic::MEAN);

    /*
     * @brief Get the best discard statistics.
     *
     * @important This should only be called after sort_discard_stats() has been
     *           called.
     *
     * @return The best discard statistics.
     */
    const DiscardStatistics& get_best_discard_stats() const;
    const DiscardStatistics& get_discard_stats(Card discard1,
                                               Card discard2) const;
    const std::vector<DiscardStatistics>& get_all_discard_stats() const {
        return discard_stats;
    }

    std::string get_discard_stats_string(int num_discard_stats = 15) const;
    void print_discard_stats(int num_discard_stats = 15) const;

    friend std::ostream& operator<<(
      std::ostream& os, const AllDiscardStatistics& all_discard_stats);

  private:
    std::vector<DiscardStatistics> discard_stats;
};

class StatisticTable {
  public:
    StatisticTable();
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

}  // namespace cribbage

#endif  // STATISTICS_H
