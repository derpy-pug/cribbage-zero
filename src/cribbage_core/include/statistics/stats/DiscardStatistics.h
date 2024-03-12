#pragma once

#include "card.h"
#include "player.h"
#include "statistics/stat_enums.h"
#include "ScoreDistributionTable.h"

namespace cribbage {

class DiscardStatistics {
  public:
    DiscardStatistics(Card discard1, Card discard2, bool is_dealer,
                      PlayerInfo player);

    friend class GenerateDiscardStatistics;

    inline Card get_discard1() const { return discard1; }
    inline Card get_discard2() const { return discard2; }

    const ScoreDistributionTable& get_score_dist(ScoreType score_type) const;

    float get_value(ScoreType score_type, Statistic stat) const;

    float get_mean(ScoreType score_type) const;
    int get_median(ScoreType score_type) const;
    float get_variance(ScoreType score_type) const;
    float get_std_dev(ScoreType score_type) const;
    int get_max(ScoreType score_type) const;
    int get_min(ScoreType score_type) const;

    float get_heuristic() const { return heuristic; }

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
    float heuristic;

    ScoreDistributionTable score_dist_hand;
    ScoreDistributionTable score_dist_crib;
    ScoreDistributionTable score_dist_combined;
};

} // namespace cribbage
