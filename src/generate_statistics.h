#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H 

#include <string>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"


class GenerateCribStatistics : public Statistic
{
public: 
    GenerateCribStatistics(Player* dealer, Player* pone);

    void generate_all_tables() override;
    void generate_freq_tables();

private:
    void generate_max_min_tables();
    void generate_mean_tables();
    void generate_median_tables();

    void generate_variance_tables();

    void generate_std_dev_tables();

    void generate_unscored_freq_tables();
    void generate_scored_freq_tables();
    /* void generate_score_dist_tables(); */

private:
    Player* dealer;
    Player* pone;
};


class GenerateDiscardStatistics : public Statistic
{
public:
    /*
     * @param player The player to generate discard statistics for. They should 
     *             have a hand of 6 cards.
     * @param is_dealer True if the player is the dealer, false otherwise.
     * @param gen_crib_stats The crib statistics tables to use.
     */
    GenerateDiscardStatistics(Player* player, bool is_dealer, GenerateCribStatistics* gen_crib_stats);



private:
    Player* player;
    bool is_dealer;
    GenerateCribStatistics* gen_crib_stats;

};

#endif // GENERATE_STATISTICS_H 
