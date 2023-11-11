#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>
#include <vector>
#include "card.h"

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
    //~ScoreDistributionTable() = default;

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

    int get_possible_score_min() const { return min; }
    int get_possible_score_max() const { return max; }

    void clear();

  private:
    float& get_table_value(int score);
    const float& get_table_value(int score) const;

  private:
    std::vector<float>
        dist_table;  // 0-29 for single hand, -60 through 60 for combined
    int min;
    int max;
};

template <typename T>
class Table {
  public:
    /*
     * @brief Default constructor.
     */
    Table();

    /*
     * @brief Constructor with stat name used for printing.
     */
    Table(std::string stats_name);

    Table(Table&& other) = default;

    T& get_dealer_crib(int index1, int index2) {
        return table_dealer_crib[index1][index2];
    }
    T& get_opp_crib(int index1, int index2) {
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

class StatisticTable {
  public:
    StatisticTable();
    //~StatisticTable() = default;

    StatisticTable(StatisticTable&& other) = default;

    virtual void generate_all_tables() = 0;

    virtual int load_tables(std::string dirname);
    virtual void save_tables(std::string dirname);

    virtual float get_freq(Card card1, Card card2, bool is_dealer);
    virtual ScoreDistributionTable get_score_dist(Card card1, Card card2,
                                                  bool is_dealer);
    virtual float get_mean(Card card1, Card card2, bool is_dealer);
    virtual int get_median(Card card1, Card card2, bool is_dealer);
    virtual float get_variance(Card card1, Card card2, bool is_dealer);
    virtual float get_std_dev(Card card1, Card card2, bool is_dealer);
    virtual int get_max(Card card1, Card card2, bool is_dealer);
    virtual int get_min(Card card1, Card card2, bool is_dealer);

  protected:
    Table<float> freq_table;
    int freq_num_games;

    Table<ScoreDistributionTable> score_dist_table;
    bool freq_tables_generated;

    Table<float> mean_table;
    bool mean_tables_generated;

    Table<int> median_table;

    Table<float> variance_table;
    bool variance_tables_generated;

    Table<float> std_dev_table;

    Table<int> max_table;
    Table<int> min_table;
};

#endif  // STATISTICS_H
