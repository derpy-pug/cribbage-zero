#include "statistics/stats/AllDiscardsStatistics.h"
#include <algorithm>

namespace cribbage {

AllDiscardStatistics::AllDiscardStatistics()
    : discard_stats(), score_type(), stat() {}

const DiscardStatistics& AllDiscardStatistics::get_discard_stats(
  Card discard1, Card discard2) const {
    for (auto it = discard_stats.cbegin(); it != discard_stats.cend(); ++it) {
        if ((*it).get_discard1() == discard1 &&
            (*it).get_discard2() == discard2) {
            return *it;
        }
    }
    std::cerr << "Error: Discard statistics not found" << std::endl;
    throw std::exception();
}

void AllDiscardStatistics::sort(ScoreType score_type, Statistic stat) {
    this->score_type = score_type;
    this->stat = stat;

    auto comparison_heuristic = [score_type](const DiscardStatistics& a,
                                             const DiscardStatistics& b) {
        return a.get_mean(score_type) > b.get_mean(score_type);
    };

    switch (stat) {
        case Statistic::MEAN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             [score_type](const DiscardStatistics& a,
                                          const DiscardStatistics& b) {
                                 return a.get_mean(score_type) >
                                        b.get_mean(score_type);
                             });
            break;
        case Statistic::MEDIAN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::VARIANCE:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::STD_DEV:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::MAX:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::MIN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        default:
            std::cerr << "Error: Invalid statistic" << std::endl;
            exit(1);
    }
}

const DiscardStatistics& AllDiscardStatistics::get_best_discard_stats() const {
    return discard_stats[0];
}

std::string AllDiscardStatistics::get_discard_stats_string(
  std::optional<int> num_discard_stats) const {
    int num = 15;  // Default
    if (num_discard_stats)
        num = *num_discard_stats;

    std::string discard_stats_string = "";
    for (int i = 0; i < num; i++) {
        discard_stats_string += discard_stats[i].get_discard_string();
        discard_stats_string += "\n";
    }
    return discard_stats_string;
}

void AllDiscardStatistics::print_discard_stats(int num_discard_stats) const {
    std::cout << get_discard_stats_string(num_discard_stats);
}

std::ostream& operator<<(std::ostream& os,
                         const AllDiscardStatistics& gen_discard_stats) {
    for (auto it = gen_discard_stats.discard_stats.cbegin();
         it != gen_discard_stats.discard_stats.cend(); ++it) {
        os << *it << std::endl;
    }
    return os;
}

} // namespace cribbage
