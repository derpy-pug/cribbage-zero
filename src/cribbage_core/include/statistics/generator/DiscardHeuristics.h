#pragma once

#include "statistics/stats/AllDiscardsStatistics.h"
#include "statistics/stats/DiscardHeuristics.h"

namespace cribbage {

class GenerateDiscardHeuristics {
  public:
    GenerateDiscardHeuristics(const AllDiscardStatistics& all_discard_stats); 

    DiscardHeuristics generate_discard_heuristics(ScoreType score_type = ScoreType::COMBINED,
                                                  Statistic stat = Statistic::MEAN) const;

  private:
    const AllDiscardStatistics& all_discard_stats;
};

}  // namespace cribbage
