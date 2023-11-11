#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H

#include <memory>
#include <optional>
#include <string>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"

class GenerateCribStatistics : public StatisticTable {
  public:
    GenerateCribStatistics(Player* dealer, Player* pone);

    void generate_all_tables() override;

    float get_mean_counting_flush(Card card1, Card card2, bool is_dealer,
                                  int num_flush_cards);

  private:
    void generate_all_but_freq_tables();

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

class DiscardStatistics {
  public:
    DiscardStatistics(Player* player, Card discard1, Card discard2,
                      bool is_dealer, GenerateCribStatistics* gen_crib_stats);
    ~DiscardStatistics() = default;

    DiscardStatistics(DiscardStatistics&& other) = default;

    void generate_all_tables(std::optional<Card> cut = std::nullopt);

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
    Player* player;
    bool is_dealer;
    GenerateCribStatistics* gen_crib_stats;

    ScoreDistributionTable score_dist_hand;
    ScoreDistributionTable score_dist_crib;
    ScoreDistributionTable score_dist_combined;
};

class GenerateDiscardStatistics {
  public:
    /*
     * @param player The player to generate discard statistics for. They should 
     *             have a hand of 6 cards.
     * @param is_dealer True if the player is the dealer, false otherwise.
     * @param gen_crib_stats The crib statistics tables to use.
     */
    GenerateDiscardStatistics(Player* player, bool is_dealer,
                              GenerateCribStatistics* gen_crib_stats);
    ~GenerateDiscardStatistics() = default;

    void generate_discard_stats(std::optional<Card> cut = std::nullopt);

    const DiscardStatistics& get_discard_stats(Card discard1,
                                               Card discard2) const;

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

    std::string get_discard_stats_string(int num_discard_stats = 15) const;
    void print_discard_stats(int num_discard_stats = 15) const;

    friend std::ostream& operator<<(
        std::ostream& os, const GenerateDiscardStatistics& gen_discard_stats);

  private:
    std::vector<std::unique_ptr<DiscardStatistics>> discard_stats;
    Player* player;
    bool is_dealer;
    GenerateCribStatistics* gen_crib_stats;
};

#endif  // GENERATE_STATISTICS_H
