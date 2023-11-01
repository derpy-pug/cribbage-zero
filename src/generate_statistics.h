#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H 

#include <string>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"

class GenerateStatistics
{
public: 
    GenerateStatistics(Player* dealer, Player* pone);

    void generate_mean_tables();
    void generate_freq_tables();
    void generate_max_min_tables();

    void load_mean_tables(std::string filename);
    void save_mean_tables(std::string filename);

private:
    void generate_unscored_freq_tables();
    void generate_scored_freq_tables();
    void generate_score_dist_tables();

private:
    int freq_table_my_crib[13][13];
    int freq_table_opp_crib[13][13];
    int num_games;

    ScoreFreqTable score_freq_table_my_crib[13][13];
    ScoreFreqTable score_freq_table_opp_crib[13][13];

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

    int max_table[13][13];
    int min_table[13][13];

    Player* dealer;
    Player* pone;
};

#endif // GENERATE_STATISTICS_H 
