#pragma once

#include <vector>

#include "card.h"
#include "DiscardStatistics.h"

namespace cribbage {

class DiscardHeuristics {
  public:
    DiscardHeuristics() = default;

    friend class GenerateDiscardHeuristics;

    float get_heuristic(Card discard1, Card discard2) const;
    float get_heuristic(const DiscardStatistics& discard_stats) const;

  private:
    int get_index(Card discard1, Card discard2) const;
    int get_index(std::pair<Card, Card> discards) const;

  private:
    std::vector<float> heuristics;
    std::vector<std::pair<Card, Card>> discards;
};

} // namespace cribbage
