#include "statistics/stats/DiscardHeuristics.h"

namespace cribbage {

int DiscardHeuristics::get_index(Card discard1, Card discard2) const {
    auto pair = std::pair<Card, Card>(discard1, discard2);
    return get_index(pair);
}

int DiscardHeuristics::get_index(std::pair<Card, Card> discard) const {
    for (auto it = discards.cbegin(); it != discards.cend(); ++it) {
        if (*it == discard || std::make_pair(it->second, it->first) == discard) {
            return std::distance(discards.cbegin(), it);
        }
    } 
    return -1;
}

float DiscardHeuristics::get_heuristic(Card discard1, Card discard2) const {
    int index = get_index(discard1, discard2);
    if (index == -1) {
        std::cerr << "Error: Discard not found" << std::endl;
        exit(1);
    }
    return heuristics[index];
}

float DiscardHeuristics::get_heuristic(const DiscardStatistics& discard_stats) const {
    return get_heuristic(discard_stats.get_discard1(), discard_stats.get_discard2());
}

} // namespace cribbage
