#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H 

#include <string>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"

template <typename T>
class Table
{
public:
    Table()
    {
        for (int i = 0; i < 13; ++i)
        {
            for (int j = 0; j < 13; ++j)
            {
                table[i][j] = T();
            }
        }
    }

    T& operator()(int i, int j)
    {
        return table[i][j];
    }

    T& operator[](int i)
    {
        return table[i];
    }

    const T& operator()(int i, int j) const
    {
        return table[i][j];
    }

    const T& operator[](int i) const
    {
        return table[i];
    }

    void save(std::string filename);
    void load(std::string filename);

private:
    T table[13][13];
};

class GenerateCribStatistics
{
public: 
    GenerateCribStatistics(Player* dealer, Player* pone);

    void generate_mean_tables();
    void generate_freq_tables();
    void generate_max_min_tables();

    void load_mean_tables(std::string filename);
    void save_mean_tables(std::string filename);

    void load_freq_tables(std::string filename);
    void save_freq_tables(std::string filename);

private:
    void generate_unscored_freq_tables();
    void generate_scored_freq_tables();
    /* void generate_score_dist_tables(); */

private:
    float freq_table_my_crib[13][13];
    float freq_table_opp_crib[13][13];
    int num_games;

    ScoreDistributionTable score_dist_table_my_crib[13][13];
    ScoreDistributionTable score_dist_table_opp_crib[13][13];
    bool freq_tables_generated;

    /*
     * @brief Given two discards, returns the expected value of the crib.
     *
     * If it is the player's crib, use mean_table_my_crib.
     * If it is the opponent's crib, use mean_table_opp_crib.
     */
    float mean_table_my_crib[13][13];
    float mean_table_opp_crib[13][13];

    int max_table_my_crib[13][13];
    int min_table_my_crib[13][13];
    int max_table_opp_crib[13][13];
    int min_table_opp_crib[13][13];

    Player* dealer;
    Player* pone;
};

#endif // GENERATE_STATISTICS_H 
