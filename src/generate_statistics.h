#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H 

#include <string>
#include <memory>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"


class GenerateCribStatistics : public Statistic
{
public: 
    GenerateCribStatistics(Player* dealer, Player* pone);
    ~GenerateCribStatistics() override = default;

    void generate_all_tables() override;

    float get_mean_counting_flush(Card card1, Card card2, bool is_dealer, int num_flush_cards);

private:
    void generate_freq_tables();

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

class DiscardStatistics
{
public:
    DiscardStatistics(Player* player, Card discard1, Card discard2, bool is_dealer, GenerateCribStatistics* gen_crib_stats);
    ~DiscardStatistics() = default;

    DiscardStatistics(DiscardStatistics&& other) = default;


    void generate_all_tables();

    inline Card get_discard1() const { return discard1; }
    inline Card get_discard2() const { return discard2; }

private:
    /* void generate_score_dist_tables(); */

private:
    Card discard1;
    Card discard2;
    Player* player;
    bool is_dealer;
    GenerateCribStatistics* gen_crib_stats;

    ScoreDistributionTable score_dist_hand;
    ScoreDistributionTable score_dist_combined;
};

class GenerateDiscardStatistics 
{
public:
    /*
     * @param player The player to generate discard statistics for. They should 
     *             have a hand of 6 cards.
     * @param is_dealer True if the player is the dealer, false otherwise.
     * @param gen_crib_stats The crib statistics tables to use.
     */
    GenerateDiscardStatistics(Player* player, bool is_dealer, GenerateCribStatistics* gen_crib_stats);
    ~GenerateDiscardStatistics() = default;

    void generate_discard_stats();

private:
    std::vector<std::unique_ptr<DiscardStatistics>> discard_stats;
    Player* player;
    bool is_dealer;
    GenerateCribStatistics* gen_crib_stats;

};

#endif // GENERATE_STATISTICS_H 
