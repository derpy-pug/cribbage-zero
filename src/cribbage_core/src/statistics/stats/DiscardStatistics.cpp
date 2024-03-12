#include "statistics/stats/DiscardStatistics.h"

namespace cribbage {

DiscardStatistics::DiscardStatistics(Card discard1, Card discard2,
                                     bool is_dealer, PlayerInfo player_info)

    : discard1(discard1),
      discard2(discard2),
      player_info(player_info),
      is_dealer(is_dealer),
      heuristic(0),
      score_dist_hand(),
      score_dist_crib(),
      score_dist_combined(-60, 60) {}

ScoreDistributionTable& DiscardStatistics::get_score_dist(
  ScoreType score_type) {
    switch (score_type) {
        case ScoreType::HAND:
            return score_dist_hand;
        case ScoreType::CRIB:
            return score_dist_crib;
        case ScoreType::COMBINED:
            return score_dist_combined;
        default:
            std::cerr << "Error: Invalid score type" << std::endl;
            exit(1);
    }
}

const ScoreDistributionTable& DiscardStatistics::get_score_dist(
  ScoreType score_type) const {
    switch (score_type) {
        case ScoreType::HAND:
            return score_dist_hand;
        case ScoreType::CRIB:
            return score_dist_crib;
        case ScoreType::COMBINED:
            return score_dist_combined;
        default:
            std::cerr << "Error: Invalid score type" << std::endl;
            exit(1);
    }
}

float DiscardStatistics::get_value(ScoreType score_type, Statistic stat) const {
    switch (stat) {
        case Statistic::MEAN:
            return get_mean(score_type);
        case Statistic::MEDIAN:
            return get_median(score_type);
        case Statistic::VARIANCE:
            return get_variance(score_type);
        case Statistic::STD_DEV:
            return get_std_dev(score_type);
        case Statistic::MAX:
            return get_max(score_type);
        case Statistic::MIN:
            return get_min(score_type);
        default:
            std::cerr << "Error: Invalid statistic" << std::endl;
            exit(1);
    }
}

float DiscardStatistics::get_mean(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_mean();
}

int DiscardStatistics::get_median(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_median();
}

float DiscardStatistics::get_variance(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_variance();
}

float DiscardStatistics::get_std_dev(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_std_dev();
}

int DiscardStatistics::get_max(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_max();
}

int DiscardStatistics::get_min(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_min();
}

float DiscardStatistics::get_prob(ScoreType score_type, int score) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score(score);
}

float DiscardStatistics::get_prob_cummulative(ScoreType score_type,
                                              int score) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score_cummulative(score);
}

float DiscardStatistics::get_prob_between(ScoreType score_type, int score1,
                                          int score2) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score_between(score1, score2);
}

std::string DiscardStatistics::get_discard_string() const {
    std::string discard_string = "Discards: ";
    discard_string += discard1.to_string();
    discard_string += " ";
    discard_string += discard2.to_string();
    discard_string += "\n";

    discard_string += "Hand Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::HAND)) + " ";
    discard_string += "Min: " + std::to_string(get_min(ScoreType::HAND)) + " ";
    discard_string += "Max: " + std::to_string(get_max(ScoreType::HAND)) + " ";
    discard_string += "\n";

    discard_string += "Crib Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::CRIB)) + " ";
    discard_string += "Min: " + std::to_string(get_min(ScoreType::CRIB)) + " ";
    discard_string += "Max: " + std::to_string(get_max(ScoreType::CRIB)) + " ";
    discard_string += "\n";

    discard_string += "Combined Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::COMBINED)) + " ";
    discard_string +=
      "Min: " + std::to_string(get_min(ScoreType::COMBINED)) + " ";
    discard_string +=
      "Max: " + std::to_string(get_max(ScoreType::COMBINED)) + " ";
    discard_string += "\n";

    discard_string += "Heuristic: " + std::to_string(heuristic) + "\n";
    return discard_string;
}

std::ostream& operator<<(std::ostream& os,
                         const DiscardStatistics& discard_stats) {
    os << discard_stats.get_discard_string();
    return os;
}

} // namespace cribbage
