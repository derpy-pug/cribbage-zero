#pragma once

#include <string>
#include <utility>

#include "hand.h"
#include "statistics.h"

namespace cribbage {

Hand string_to_hand(std::string hand_str);

std::pair<Card, Card> string_to_discards(std::string discard_str);

std::pair<Statistic, ScoreType> string_to_sort_by(std::string sort_by_str);

}  // namespace cribbage
