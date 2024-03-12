#pragma once

#include <optional>
#include <string>
#include <vector>

#include "statistics/stat_enums.h"
#include "DiscardStatistics.h"

namespace cribbage {

class AllDiscardStatistics {
  public:
    AllDiscardStatistics();

    friend class GenerateDiscardStatistics;

    void sort(ScoreType score_type = ScoreType::COMBINED,
                            Statistic stat = Statistic::MEAN);

    /*
     * @brief Get the best discard statistics.
     *
     * @important This should only be called after sort() has been
     *           called.
     *
     * @return The best discard statistics.
     */
    const DiscardStatistics& get_best_discard_stats() const;
    const DiscardStatistics& get_discard_stats(Card discard1,
                                               Card discard2) const;
    const std::vector<DiscardStatistics>& get_all_discard_stats() const {
        return discard_stats;
    }

    std::string get_discard_stats_string(
      std::optional<int> num_discard_stats) const;
    void print_discard_stats(int num_discard_stats = 15) const;

    friend std::ostream& operator<<(
      std::ostream& os, const AllDiscardStatistics& all_discard_stats);

  private:
    std::vector<DiscardStatistics> discard_stats;

    ScoreType score_type; 
    Statistic stat;
};

} // namespace cribbage
