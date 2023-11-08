#ifndef STATISTICS_H
#define STATISTICS_H

#include "card.h"
#include <string>
struct ScoreDistributionTable
{
    ScoreDistributionTable()
        : dist_table()
    {
    }
    float dist_table[30]; // 0-29

    float& operator[](int i)
    {
        return dist_table[i];
    }
};

template <typename T>
class Table
{
public:
    /*
     * @brief Default constructor.
     */
    Table();

    /*
     * @brief Constructor with stat name used for printing.
     */
    Table(std::string stats_name);

    T& get_dealer_crib(int index1, int index2) { return table_dealer_crib[index1][index2]; }
    T& get_opp_crib(int index1, int index2) { return table_opp_crib[index1][index2]; }

    void pretty_print();

    void save(std::string filename);
    void load(std::string filename);

private:
    std::string stats_name;
    T table_dealer_crib[13][13];
    T table_opp_crib[13][13];
};


class Statistic
{
public:
    Statistic();
    
    virtual void generate_all_tables() = 0;

    virtual void load_tables(std::string dirname);
    virtual void save_tables(std::string dirname);

    virtual int get_freq(Card card1, Card card2, bool is_dealer);
    virtual ScoreDistributionTable get_score_dist(Card card1, Card card2, bool is_dealer);
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

#endif // STATISTICS_H
